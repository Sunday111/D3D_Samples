#pragma once

#include <fstream>
#include "d3d11.h"
#include "d3dcompiler.h"
#include "EverydayTools\Array\ArrayView.h"
#include "Shader.h"
#include "Texture.h"
#include "WinWrappers/WinWrappers.h"
#include "WinWrappers/ComPtr.h"

#include "D3D_Tools/Device.h"
#include "D3D_Tools/SwapChain.h"

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

class Device :
    public RefCountImpl<IDevice>,
    public d3d_tools::Device
{
public:
    using d3d_tools::Device::Device;

    void* GetNativeDevice() const override {
        return GetDevice();
    }

    void* GetNativeContext() const override {
        return GetContext();
    }
};

class SwapChain :
    public RefCountImpl<ISwapChain>,
    public d3d_tools::SwapChain
{
public:
    using d3d_tools::SwapChain::SwapChain;

    void* GetNativeInterface() const {
        return GetInterface();
    }
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
            m_swapchain = IntrusivePtr<SwapChain>::MakeInstance(m_device->GetDevice(), params.Width, params.Height, params.hWnd);
        };
    }

    void InitializeRenderTarget(CreateParams& params) {
        CallAndRethrowM + [&] {
            UnusedVar(params);
            auto backbuffer = m_swapchain->GetBackBuffer();
            m_renderTargetView = IntrusivePtr<TextureView<ResourceViewType::RenderTarget>>::MakeInstance(
                m_device->GetDevice(),
                backbuffer.GetTexture());
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