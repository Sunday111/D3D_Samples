#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <fstream>
#include "WinWrappers\WinWrappers.h"

template<typename T>
class D3DPtr {
public:
    D3DPtr(T* p = nullptr) :
        m_p(p)
    {}

    D3DPtr(const D3DPtr&) = delete;

    D3DPtr(D3DPtr&& that) {
        MoveFrom<false>(std::move(that));
    }

    ~D3DPtr() {
        Release<false>();
    }

    T* Get() const {
        return m_p;
    }

    template<bool release_prev = true>
    void Set(T* ptr) {
        if constexpr (release_prev) {
            Release<false>();
        }
        m_p = ptr;
    }

    template<bool set_to_null>
    void Release() {
        if (m_p != nullptr) {
            m_p->Release();
            if constexpr (set_to_null) {
                m_p = nullptr;
            }
        }
    }

    template<bool release_prev = true>
    void MoveFrom(D3DPtr<T>&& that) {
        Set<release_prev>(that.m_p);
        that.m_p = nullptr;
    }

    T* operator->() const {
        return m_p;
    }

    T& operator*() const {
        return *m_p;
    }

    D3DPtr& operator=(const D3DPtr&) = delete;

    D3DPtr& operator=(D3DPtr&& that) {
        MoveFrom<true>(std::move(that));
        return *this;
    }

    T** Receive() {
        Release<true>();
        return &m_p;
    }

private:
    T * m_p;
};

enum class ShaderType {
    Compute,
    Domain,
    Geometry,
    Hull,
    Pixel,
    Vertex
};

enum class ShaderVersion {
    _5_0,
    _4_1,
    _4_0,
};


template<typename Interface_>
struct ShaderTraisBase {
    using Interface = Interface_;
    using CreateMethodType = HRESULT(ID3D11Device::*)(const void*, SIZE_T, ID3D11ClassLinkage*, Interface**);
    using SetMethodType = void(ID3D11DeviceContext::*)(Interface*, ID3D11ClassInstance*const*, unsigned);
};

template<ShaderType shaderType>
struct ShaderTraits;

template<>
struct ShaderTraits<ShaderType::Vertex> :
    public ShaderTraisBase<ID3D11VertexShader>
{
    static CreateMethodType CreateMethod() { return &ID3D11Device::CreateVertexShader; }
    static SetMethodType SetMethod() { return &ID3D11DeviceContext::VSSetShader; }
};

template<>
struct ShaderTraits<ShaderType::Pixel> :
    public ShaderTraisBase<ID3D11PixelShader>
{
    static CreateMethodType CreateMethod() { return &ID3D11Device::CreatePixelShader; }
    static SetMethodType SetMethod() { return &ID3D11DeviceContext::PSSetShader; }
};

template<>
struct ShaderTraits<ShaderType::Geometry> :
    public ShaderTraisBase<ID3D11GeometryShader>
{
    static CreateMethodType CreateMethod() { return &ID3D11Device::CreateGeometryShader; }
    static SetMethodType SetMethod() { return &ID3D11DeviceContext::GSSetShader; }
};

template<>
struct ShaderTraits<ShaderType::Hull> :
    public ShaderTraisBase<ID3D11HullShader>
{
    static CreateMethodType CreateMethod() { return &ID3D11Device::CreateHullShader; }
    static SetMethodType SetMethod() { return &ID3D11DeviceContext::HSSetShader; }
};

template<>
struct ShaderTraits<ShaderType::Domain> :
    public ShaderTraisBase<ID3D11DomainShader>
{
    static CreateMethodType CreateMethod() { return &ID3D11Device::CreateDomainShader; }
    static SetMethodType SetMethod() { return &ID3D11DeviceContext::DSSetShader; }
};

template<>
struct ShaderTraits<ShaderType::Compute> :
    public ShaderTraisBase<ID3D11ComputeShader>
{
    static CreateMethodType CreateMethod() { return &ID3D11Device::CreateComputeShader; }
    static SetMethodType SetMethod() { return &ID3D11DeviceContext::CSSetShader; }
};

template<ShaderType shaderType>
class Shader {
public:
    using Traits = ShaderTraits<shaderType>;
    using Interface = typename Traits::Interface;
    D3DPtr<ID3D10Blob> bytecode;
    D3DPtr<Interface> shader;
};

template<typename Element>
class BufferMapper {
public:
    BufferMapper(ID3D11Buffer* buffer, ID3D11DeviceContext* deviceContext, D3D11_MAP mapType, unsigned mapFlags = 0) :
        m_buffer(buffer),
        m_deviceContext(deviceContext)
    {
        Map(mapType, mapFlags);
    }

    void Write(const Element* elements, size_t count) {
        std::copy(elements, elements + count, GetDataPtr());
    }

    Element& At(size_t index) {
        return GetDataPtr()[index];
    }

    const Element& At(size_t index) const {
        return *(GetDataPtr()[index]);
    }

    Element* GetDataPtr() const {
        return (Element*)m_subresource.pData;
    }

    ~BufferMapper() {
        Unmap();
    }

protected:
    void Map(D3D11_MAP mapType, unsigned mapFlags) {
        WinAPI<char>::ThrowIfError(m_deviceContext->Map(m_buffer, 0, mapType, mapFlags, &m_subresource));
    }

    void Unmap() {
        m_deviceContext->Unmap(m_buffer, 0);
    }

private:
    D3D11_MAPPED_SUBRESOURCE m_subresource;
    ID3D11Buffer* m_buffer = nullptr;
    ID3D11DeviceContext* m_deviceContext = nullptr;
};

class Renderer {
public:
    struct CreateParams {
        HWND hWnd = nullptr;
        bool debugDevice = false;
        bool noDeviceMultithreading = false;
        int Width = 1280;
        int Height = 720;
    };

    Renderer(CreateParams& params) {
        Initialize(params);
    }

    ~Renderer() {
        if (m_swapchain.Get()) {
            m_swapchain->SetFullscreenState(false, nullptr);
        }
    }

    template<ShaderType shaderType>
    decltype(auto) CreateShaderFromFile(const char* fileName, const char* entryPoint, ShaderVersion shaderVersion) {
        return CallAndRethrow("Renderer::CreateShaderFromFile<>", [&]() {
            std::ifstream file(fileName, std::ios::binary);
            if (!file.is_open()) {
                std::string message("Could not open this file: ");
                message += fileName;
                throw std::invalid_argument(std::move(message));
            }
            return CreateShader<shaderType>(file, entryPoint, shaderVersion);
        });
    }

    template<ShaderType shaderType>
    decltype(auto) CreateShader(std::istream& code, const char* entryPoint, ShaderVersion shaderVersion) {
        return CallAndRethrow("Renderer::CreateShader<>(std::istream& code, ...)", [&]() {
            auto startpos = code.tellg();
            code.seekg(0, std::ios::end);
            auto endpos = code.tellg();
            auto size = endpos - startpos;
            code.seekg(startpos);
            std::string readcode;
            readcode.resize(size);
            code.read(&readcode[0], size);
            return CreateShader<shaderType>(readcode.data(), entryPoint, shaderVersion);
        });
    }

    template<ShaderType shaderType>
    decltype(auto) CreateShader(const char* code, const char* entryPoint, ShaderVersion shaderVersion) {
        return CallAndRethrow("Renderer::CreateShader<>(const char* code, ...)", [&]() {
            // Compile shader source to bytecode
            Shader<shaderType> result;
            result.bytecode = CompileShaderToBlob(code, entryPoint, shaderType, shaderVersion);
            using Traits = ShaderTraits<shaderType>;
            using Interface = Traits::Interface;
            auto method = Traits::CreateMethod();
            // Create shader from bytecode
            WinAPI<char>::ThrowIfError((*m_device.*method)(
                result.bytecode->GetBufferPointer(),
                result.bytecode->GetBufferSize(),
                nullptr,
                result.shader.Receive()));
            return result;
        });
    }

    template<ShaderType shaderType>
    void SetShader(Shader<shaderType>& shader) {
        CallAndRethrow("Renderer::SetShader<>", [&]() {
            using Traits = ShaderTraits<shaderType>;
            auto method = Traits::SetMethod();
            (*m_deviceContext.*method)(shader.shader.Get(), 0, 0);
        });
    }

    static D3DPtr<ID3DBlob> CompileShaderToBlob(const char* code, const char* entryPoint, ShaderType shaderType, ShaderVersion shaderVersion) {
        return CallAndRethrow("Renderer::CompileShaderToBlob", [&]() {
            auto datasize = std::char_traits<char>::length(code);
            D3D_SHADER_MACRO macro[] = { nullptr, nullptr };
            auto shaderTarget = ShaderTypeToShaderTarget(shaderType, shaderVersion);
            D3DPtr<ID3DBlob> shaderBlob;
            D3DPtr<ID3DBlob> errorBlob;
            // This lambda combines described windows error code with d3d compiler error
            // and throws and exception
            auto onCompileError = [&](std::string errorMessage) {
                if (errorBlob.Get()) {
                    errorMessage += ": ";
                    errorMessage.append(
                        (char*)errorBlob->GetBufferPointer(),
                        errorBlob->GetBufferSize());
                }
                throw std::runtime_error(std::move(errorMessage));
            };
            WinAPI<char>::HandleError(D3DCompile(
                code,
                datasize,
                nullptr,              // May be used for debugging
                macro,                // Null-terminated array of macro definitions
                nullptr,              // Includes
                entryPoint,           // Main function of shader
                shaderTarget,         // shader target
                0,                    // Flags for compile constants
                0,                    // Flags for compile effects constants
                shaderBlob.Receive(), // Output compiled shader
                errorBlob.Receive()   // Compile error messages
            ), onCompileError);
            return shaderBlob;
        });
    }

    D3DPtr<ID3D11Buffer> CreateBuffer(D3D11_BUFFER_DESC desc, void* initialData = nullptr) {
        D3DPtr<ID3D11Buffer> buffer;
        D3D11_SUBRESOURCE_DATA subresourceData;
        const D3D11_SUBRESOURCE_DATA* pSubresourceData = nullptr;
        if (initialData) {
            subresourceData.pSysMem = initialData;
            subresourceData.SysMemPitch = 0;
            subresourceData.SysMemSlicePitch = 0;
            pSubresourceData = &subresourceData;
        }
        WinAPI<char>::ThrowIfError(m_device->CreateBuffer(&desc, pSubresourceData, buffer.Receive()));
        return buffer;
    }

    ID3D11Device* GetDeivce() const {
        return m_device.Get();
    }

    ID3D11DeviceContext* GetDeviceContext() const {
        return m_deviceContext.Get();
    }

    D3DPtr<ID3D11InputLayout> CreateInputLayout(const D3D11_INPUT_ELEMENT_DESC* elementDescriptor, unsigned elementsCount, ID3D10Blob* shader) {
        return CallAndRethrow("Renderer::CreateInputLayout", [&]() {
            D3DPtr<ID3D11InputLayout> result;
            WinAPI<char>::ThrowIfError(
                m_device->CreateInputLayout(
                    elementDescriptor, elementsCount,
                    shader->GetBufferPointer(), shader->GetBufferSize(),
                    result.Receive()));
            return result;
        });
    }

    void SetInputLayout(ID3D11InputLayout* layout) {
        m_deviceContext->IASetInputLayout(layout);
    }

    void SetVertexBuffer(ID3D11Buffer* buffer, unsigned stride, unsigned offset) {
        m_deviceContext->IASetVertexBuffers(0, 1, &buffer, &stride, &offset);
    }

    void SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY topo) {
        m_deviceContext->IASetPrimitiveTopology(topo);
    }

    void Draw(unsigned vertexCount, unsigned startvert) {
        m_deviceContext->Draw(vertexCount, startvert);
    }

    void Clear(float r, float g, float b, float a) {
        float clearColor[] = { r, g, b, a };
        m_deviceContext->ClearRenderTargetView(m_renderTargetView.Get(), clearColor);
    }

    void Present(unsigned interval = 0, unsigned flags = 0) {
        m_swapchain->Present(interval, flags);
    }

protected:
    void Initialize(CreateParams& params) {
        CallAndRethrow("Renderer::Initialize", [&]() {
            InitializeD3D(params);
            InitializeRenderTarget(params);
            InitializeViewport(params);
        });
    }

    void InitializeD3D(CreateParams& params) {
        CallAndRethrow("Renderer::InitializeD3D", [&]() {
            DXGI_SWAP_CHAIN_DESC scd{};
            scd.BufferCount = 1;                                    // one back buffer
            scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;     // use 32-bit color
            scd.BufferDesc.Width = params.Width;
            scd.BufferDesc.Height = params.Height;
            scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;      // how swap chain is to be used
            scd.OutputWindow = params.hWnd;                         // the window to be used
            scd.SampleDesc.Count = 4;                               // how many multisamples
            scd.Windowed = TRUE;                                    // windowed/full-screen mode
            scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
            unsigned flags = 0;
            if (params.debugDevice) {
                flags |= D3D11_CREATE_DEVICE_DEBUG;
            }

            if (params.noDeviceMultithreading) {
                flags |= D3D11_CREATE_DEVICE_SINGLETHREADED;
                flags |= D3D11_CREATE_DEVICE_PREVENT_INTERNAL_THREADING_OPTIMIZATIONS;
            }

            ID3D11Device* device = nullptr;
            ID3D11DeviceContext* deviceContext = nullptr;
            IDXGISwapChain* swapchain = nullptr;

            // create a device, device context and swap chain using the information in the scd struct
            WinAPI<char>::ThrowIfError(D3D11CreateDeviceAndSwapChain(NULL,
                D3D_DRIVER_TYPE_HARDWARE,
                NULL,
                flags,
                NULL,
                NULL,
                D3D11_SDK_VERSION,
                &scd,
                &swapchain,
                &device,
                NULL,
                &deviceContext));

            m_device.Set(device);
            m_deviceContext.Set(deviceContext);
            m_swapchain.Set(swapchain);
        });
    }

    void InitializeRenderTarget(CreateParams& params) {
        CallAndRethrow("Renderer::InitializeRenderTarget", [&]() {
            UnusedVar(params);
            D3DPtr<ID3D11Texture2D> backBuffer;
            // get the address of the back buffer
            WinAPI<char>::ThrowIfError(m_swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)backBuffer.Receive()));
            // use the back buffer address to create the render target
            WinAPI<char>::ThrowIfError(m_device->CreateRenderTargetView(backBuffer.Get(), nullptr, m_renderTargetView.Receive()));
            // set the render target as the back buffer
            auto renderTargetView = m_renderTargetView.Get();
            m_deviceContext->OMSetRenderTargets(1, &renderTargetView, nullptr);
        });
    }

    void InitializeViewport(CreateParams& params) {
        CallAndRethrow("Renderer::InitializeViewport", [&]() {
            D3D11_VIEWPORT viewport{};
            viewport.TopLeftX = 0;
            viewport.TopLeftY = 0;
            viewport.Width = static_cast<float>(params.Width);
            viewport.Height = static_cast<float>(params.Height);
            m_deviceContext->RSSetViewports(1, &viewport);
        });
    }

    void Render() {
        float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
        // clear the back buffer to a deep blue
        m_deviceContext->ClearRenderTargetView(m_renderTargetView.Get(), clearColor);
        // do 3D rendering on the back buffer here
        ///...
        // switch the back buffer and the front buffer
        Present();
    }
    
private:
    static const char* ShaderTypeToShaderTarget(ShaderType shaderType, ShaderVersion shaderVersion) {
        return CallAndRethrow("Renderer::ShaderTypeToShaderTarget", [&]() {
            switch (shaderVersion)
            {
            case ShaderVersion::_5_0:
                switch (shaderType)
                {
                    case ShaderType::Compute: return "cs_5_0";
                    case ShaderType::Domain:        return "ds_5_0";
                    case ShaderType::Geometry:      return "gs_5_0";
                    case ShaderType::Hull:          return "hs_5_0";
                    case ShaderType::Pixel:         return "ps_5_0";
                    case ShaderType::Vertex:        return "vs_5_0";
                    default: throw std::invalid_argument("This version (5.0) does not support this shader type");
                }
            case ShaderVersion::_4_1:
                switch (shaderType)
                {
                    case ShaderType::Compute: return "cs_4_1";
                    case ShaderType::Geometry:      return "gs_4_1";
                    case ShaderType::Pixel:         return "ps_4_1";
                    case ShaderType::Vertex:        return "vs_4_1";
                    default: throw std::invalid_argument("This version (4.1) does not support this shader type");
                }
            case ShaderVersion::_4_0:
                switch (shaderType)
                {
                    case ShaderType::Compute: return "cs_4_0";
                    case ShaderType::Geometry:      return "gs_4_0";
                    case ShaderType::Pixel:         return "ps_4_0";
                    case ShaderType::Vertex:        return "vs_4_0";
                    default: throw std::invalid_argument("This version (4.0) does not support this shader type");
                }
            default:
                throw std::invalid_argument("This version is not supported");
            }
        });
    }

    bool m_fullscreen = false;
    D3DPtr<ID3D11Device> m_device;
    D3DPtr<ID3D11DeviceContext> m_deviceContext;
    D3DPtr<IDXGISwapChain> m_swapchain;
    D3DPtr<ID3D11RenderTargetView> m_renderTargetView;
};