#pragma once

#include <fstream>
#include <d3d11.h>
#include <d3dcompiler.h>
#include "EverydayTools\Array\ArrayView.h"
#include "WinWrappers\WinWrappers.h"
#include "ComPtr.h"
#include "Shader.h"
#include "Texture.h"

class IDevice : public IRefCountObject {
public:
    virtual void* GetNativeDevice() const = 0;
    virtual void* GetNativeContext() const = 0;
    virtual ~IDevice() = default;
};

class ISwapChain : public IRefCountObject {
public:
    virtual void* GetNativeInterface() const = 0;
    virtual ~ISwapChain() = default;
};

class Device : public RefCountImpl<IDevice>
{
public:
    struct CreateParams {
        bool debugDevice;
        bool noDeviceMultithreading = false;
    };

    Device(CreateParams params) {
        CallAndRethrowM + [&] {
            unsigned flags = 0;
            if (params.debugDevice) {
                flags |= D3D11_CREATE_DEVICE_DEBUG;
            }

            if (params.noDeviceMultithreading) {
                flags |= D3D11_CREATE_DEVICE_SINGLETHREADED;
                flags |= D3D11_CREATE_DEVICE_PREVENT_INTERNAL_THREADING_OPTIMIZATIONS;
            }

            WinAPI<char>::ThrowIfError(D3D11CreateDevice(
                nullptr,
                D3D_DRIVER_TYPE_HARDWARE,
                nullptr,
                flags,
                nullptr,
                0,
                D3D11_SDK_VERSION,
                m_device.Receive(),
                nullptr,
                m_deviceContext.Receive()));
        };
    }

    void* GetNativeDevice() const override {
        return m_device.Get();
    }

    ID3D11Device* GetDevice() const {
        return m_device.Get();
    }

    void* GetNativeContext() const override {
        return m_deviceContext.Get();
    }

    ID3D11DeviceContext* GetContext() const {
        return m_deviceContext.Get();
    }

    template<ShaderType shaderType>
    decltype(auto) CreateShader(const char* code, const char* entryPoint, ShaderVersion shaderVersion) {
        return CallAndRethrowM + [&] {
            Shader<shaderType> result;
            result.Compile(code, entryPoint, shaderVersion);
            result.Create(m_device.Get());
            return result;
        };
    }

    template<ShaderType shaderType>
    decltype(auto) CreateShader(edt::DenseArrayView<const uint8_t> code, const char* entryPoint, ShaderVersion shaderVersion) {
        return CreateShader<shaderType>((const char*)code.GetData(), entryPoint, shaderVersion);
    }

    template<ShaderType shaderType>
    decltype(auto) CreateShader(std::istream& code, const char* entryPoint, ShaderVersion shaderVersion) {
        return CallAndRethrowM + [&] {
            auto startpos = code.tellg();
            code.seekg(0, std::ios::end);
            auto endpos = code.tellg();
            auto size = endpos - startpos;
            code.seekg(startpos);
            std::string readcode;
            readcode.resize(size);
            code.read(&readcode[0], size);
            return CreateShader<shaderType>(readcode.data(), entryPoint, shaderVersion);
        };
    }

    template<ShaderType shaderType>
    void SetShader(Shader<shaderType>& shader) {
        CallAndRethrowM + [&] {
            using Traits = shader_details::ShaderTraits<shaderType>;
            Traits::Set(
                m_deviceContext.Get(),
                shader.shader.Get(),
                nullptr,
                0);
        };
    }

    void SetRenderTarget(TextureView<ResourceViewType::RenderTarget>* rtv, TextureView<ResourceViewType::DepthStencil>* dsv = nullptr) {
        auto pRTV = rtv->GetView();
        ID3D11DepthStencilView* pDSV = nullptr;
        if (dsv) {
            pDSV = dsv->GetView();
        }
        m_deviceContext->OMSetRenderTargets(1, &pRTV, pDSV);
    }

private:
    ComPtr<ID3D11Device> m_device;
    ComPtr<ID3D11DeviceContext> m_deviceContext;
};

class SwapChain : public RefCountImpl<ISwapChain>
{
public:
    SwapChain(IntrusivePtr<IDevice> device, uint32_t w, uint32_t h, HWND hWnd) :
        m_device(device)
    {
        CallAndRethrowM + [&] {
            ComPtr<IDXGIFactory> factory;
            WinAPI<char>::ThrowIfError(CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)(factory.Receive())));

            DXGI_SWAP_CHAIN_DESC scd{};
            scd.BufferCount = 1;                                    // one back buffer
            scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;     // use 32-bit color
            scd.BufferDesc.Width = w;
            scd.BufferDesc.Height = h;
            scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;      // how swap chain is to be used
            scd.OutputWindow = hWnd;                                // the window to be used
            scd.SampleDesc.Count = 1;                               // how many multisamples
            scd.Windowed = TRUE;                                    // windowed/full-screen mode
            scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
            auto pDevice = static_cast<ID3D11Device*>(device->GetNativeDevice());
            WinAPI<char>::ThrowIfError(factory->CreateSwapChain(pDevice, &scd, m_swapchain.Receive()));
        };
    }

    void* GetNativeInterface() const {
        return m_swapchain.Get();
    }

    IDXGISwapChain* GetInterface() const {
        return m_swapchain.Get();
    }

    ComPtr<ID3D11RenderTargetView> MakeRenderTargetView() {
        return CallAndRethrowM + [&] {
            ComPtr<ID3D11Texture2D> backBuffer;
            // get the address of the back buffer
            WinAPI<char>::ThrowIfError(m_swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)backBuffer.Receive()));
            // use the back buffer address to create the render target
            auto device = static_cast<ID3D11Device*>(m_device->GetNativeDevice());
            ComPtr<ID3D11RenderTargetView> result;
            WinAPI<char>::ThrowIfError(device->CreateRenderTargetView(backBuffer.Get(), nullptr, result.Receive()));
            return result;
        };
    }

private:
    IntrusivePtr<IDevice> m_device;
    ComPtr<IDXGISwapChain> m_swapchain;
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
            m_swapchain->GetInterface()->SetFullscreenState(false, nullptr);
        }
    }

    IntrusivePtr<Device> GetDevice() const {
        return m_device;
    }

    std::unique_ptr<Texture> CreateTexture(uint32_t w, uint32_t h, TextureFormat format, TextureFlags flags) {
        return CallAndRethrowM + [&] {
            return std::make_unique<Texture>(m_device->GetDevice(), w, h, format, flags);
        };
    }

    template<ResourceViewType type>
    decltype(auto) CreateTextureView(Texture* texture, TextureFormat format) {
        return CallAndRethrowM + [&] {
            return texture->GetView<type>(m_device->GetDevice(), format);
        };
    }

    ComPtr<ID3D11Buffer> CreateBuffer(D3D11_BUFFER_DESC desc, void* initialData = nullptr) {
        ComPtr<ID3D11Buffer> buffer;
        D3D11_SUBRESOURCE_DATA subresourceData;
        const D3D11_SUBRESOURCE_DATA* pSubresourceData = nullptr;
        if (initialData) {
            subresourceData.pSysMem = initialData;
            subresourceData.SysMemPitch = 0;
            subresourceData.SysMemSlicePitch = 0;
            pSubresourceData = &subresourceData;
        }
        WinAPI<char>::ThrowIfError(m_device->GetDevice()->CreateBuffer(&desc, pSubresourceData, buffer.Receive()));
        return buffer;
    }

    ComPtr<ID3D11InputLayout> CreateInputLayout(const D3D11_INPUT_ELEMENT_DESC* elementDescriptor, unsigned elementsCount, ID3D10Blob* shader) {
        return CallAndRethrowM + [&] {
            ComPtr<ID3D11InputLayout> result;
            WinAPI<char>::ThrowIfError(
                m_device->GetDevice()->CreateInputLayout(
                    elementDescriptor, elementsCount,
                    shader->GetBufferPointer(), shader->GetBufferSize(),
                    result.Receive()));
            return result;
        };
    }

    void SetInputLayout(ID3D11InputLayout* layout) {
        m_device->GetContext()->IASetInputLayout(layout);
    }

    void SetVertexBuffer(ID3D11Buffer* buffer, unsigned stride, unsigned offset) {
        m_device->GetContext()->IASetVertexBuffers(0, 1, &buffer, &stride, &offset);
    }

    void SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY topo) {
        m_device->GetContext()->IASetPrimitiveTopology(topo);
    }

    void Draw(unsigned vertexCount, unsigned startvert) {
        m_device->GetContext()->Draw(vertexCount, startvert);
    }

    void Present(unsigned interval = 0, unsigned flags = 0) {
        m_swapchain->GetInterface()->Present(interval, flags);
    }

    void SetViewport(D3D11_VIEWPORT viewport) {
        CallAndRethrowM + [&] {
            m_device->GetContext()->RSSetViewports(1, &viewport);
        };
    }

    decltype(auto) GetSwapchainRenderTargetView() {
        return m_renderTargetView;
    }

protected:
    void Initialize(CreateParams& params) {
        CallAndRethrowM + [&] {
            InitializeD3D(params);
            InitializeRenderTarget(params);
            InitializeViewport(params);
        };
    }

    void InitializeD3D(CreateParams& params) {
        CallAndRethrowM + [&] {
            Device::CreateParams deviceParams;
            deviceParams.debugDevice = params.debugDevice;
            deviceParams.noDeviceMultithreading = params.noDeviceMultithreading;
            m_device = IntrusivePtr<Device>::MakeInstance(deviceParams);
            m_swapchain = IntrusivePtr<SwapChain>::MakeInstance(m_device, params.Width, params.Height, params.hWnd);
        };
    }

    void InitializeRenderTarget(CreateParams& params) {
        CallAndRethrowM + [&] {
            UnusedVar(params);
            m_renderTargetView = IntrusivePtr<TextureView<ResourceViewType::RenderTarget>>::MakeInstance(m_swapchain->MakeRenderTargetView());
        };
    }

    void InitializeViewport(CreateParams& params) {
        CallAndRethrowM + [&] {
            D3D11_VIEWPORT viewport{};
            viewport.TopLeftX = 0;
            viewport.TopLeftY = 0;
            viewport.Width = static_cast<float>(params.Width);
            viewport.Height = static_cast<float>(params.Height);
            SetViewport(viewport);
        };
    }
    
private:

    bool m_fullscreen = false;
    IntrusivePtr<Device> m_device;
    IntrusivePtr<SwapChain> m_swapchain;
    IntrusivePtr<TextureView<ResourceViewType::RenderTarget>> m_renderTargetView;
};