#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
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

    T* Get() {
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

class Renderer {
public:
    struct CreateParams {
        HWND hWnd = nullptr;
        bool debugDevice = false;
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

            ID3D11Device* device = nullptr;
            ID3D11DeviceContext* deviceContext = nullptr;
            IDXGISwapChain* swapchain = nullptr;

            // create a device, device context and swap chain using the information in the scd struct
            WinAPI<char>::ThrowIfError(D3D11CreateDeviceAndSwapChain(NULL,
                D3D_DRIVER_TYPE_HARDWARE,
                NULL,
                NULL,
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
            ID3D11RenderTargetView* renderTargetView = nullptr;
            WinAPI<char>::ThrowIfError(m_device->CreateRenderTargetView(backBuffer.Get(), nullptr, &renderTargetView));
            m_renderTargetView.Set(renderTargetView);
            // set the render target as the back buffer
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
        m_swapchain->Present(0, 0);
    }

    template<ShaderType shaderType>
    struct ShaderTraits;

    template<>
    struct ShaderTraits<ShaderType::Vertex> {
        using Interface = ID3D11VertexShader;
        using CreateMethodType = HRESULT(ID3D11Device::*)(const void*, SIZE_T, ID3D11ClassLinkage*, Interface**);
        static CreateMethodType CreateMethod() { return &ID3D11Device::CreateVertexShader; }
    };

    template<>
    struct ShaderTraits<ShaderType::Pixel> {
        using Interface = ID3D11PixelShader;
        using CreateMethodType = HRESULT(ID3D11Device::*)(const void*, SIZE_T, ID3D11ClassLinkage*, Interface**);
        static CreateMethodType CreateMethod() { return &ID3D11Device::CreatePixelShader; }
    };

    template<>
    struct ShaderTraits<ShaderType::Geometry> {
        using Interface = ID3D11GeometryShader;
        using CreateMethodType = HRESULT(ID3D11Device::*)(const void*, SIZE_T, ID3D11ClassLinkage*, Interface**);
        static CreateMethodType CreateMethod() { return &ID3D11Device::CreateGeometryShader; }
    };

    template<>
    struct ShaderTraits<ShaderType::Hull> {
        using Interface = ID3D11HullShader;
        using CreateMethodType = HRESULT(ID3D11Device::*)(const void*, SIZE_T, ID3D11ClassLinkage*, Interface**);
        static CreateMethodType CreateMethod() { return &ID3D11Device::CreateHullShader; }
    };

    template<>
    struct ShaderTraits<ShaderType::Domain> {
        using Interface = ID3D11DomainShader;
        using CreateMethodType = HRESULT(ID3D11Device::*)(const void*, SIZE_T, ID3D11ClassLinkage*, Interface**);
        static CreateMethodType CreateMethod() { return &ID3D11Device::CreateDomainShader; }
    };

    template<>
    struct ShaderTraits<ShaderType::Compute> {
        using Interface = ID3D11ComputeShader;
        using CreateMethodType = HRESULT(ID3D11Device::*)(const void*, SIZE_T, ID3D11ClassLinkage*, Interface**);
        static CreateMethodType CreateMethod() { return &ID3D11Device::CreateComputeShader; }
    };

    template<ShaderType shaderType>
    decltype(auto) CreateShader(const char* filename, const char* entryPoint, ShaderVersion shaderVersion) {
        return CallAndRethrow("Renderer::CreateShader<>", [&]() {
            auto bytecodeBlob = CompileShaderToBlob(filename, entryPoint, shaderType, shaderVersion);
            using Traits = ShaderTraits<shaderType>;
            using Interface = Traits::Interface;
            auto method = Traits::CreateMethod();
            D3DPtr<Interface> result;
            WinAPI<char>::ThrowIfError((*m_device.*method)(
                bytecodeBlob->GetBufferPointer(),
                bytecodeBlob->GetBufferSize(),
                nullptr,
                result.Receive()));
            return result;
        });
    }

    static D3DPtr<ID3DBlob> CompileShaderToBlob(const char* filename, const char* entryPoint, ShaderType shaderType, ShaderVersion shaderVersion) {
        return CallAndRethrow("Renderer::CompileShaderToBlob", [&]() {
            auto datasize = std::char_traits<char>::length(filename);
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
                filename,
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