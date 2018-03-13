#pragma once

#include "SystemsApp/ISystem.h"
#include "WindowSystem/MainWindow.h"
#include "ResourceSystem/FileResource.h"
#include "EverydayTools/Observable.h"

#include "Rendering/Shader.h"
#include "Rendering/Texture.h"

#include "D3D_Tools/Device.h"
#include "D3D_Tools/SwapChain.h"
#include "D3D_Tools/Texture.h"
#include "D3D_Tools/Shader.h"
#include "D3D_Tools/BufferMapper.h"

class Application;

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

class IGraphicsListener {
public:
    virtual void BeforeRendering() = 0;
    virtual void AfterRendering() = 0;
    virtual ~IGraphicsListener() = default;
};

class IGpuBuffer
{
public:
    virtual ~IGpuBuffer() = default;
    virtual void Activate(Device* device, uint32_t offset = 0) = 0;
};

template<typename ElementType>
class GpuBuffer : public IGpuBuffer
{
public:
    GpuBuffer(
        IntrusivePtr<Device> device,
        D3D_PRIMITIVE_TOPOLOGY topology,
        edt::DenseArrayView<const ElementType> elements) :
        m_topology(topology)
    {
        auto count = elements.GetSize();
        if (count == 0) {
            return;
        }

        m_cpuMirror.reserve(count);
        for (auto& element : elements) {
            m_cpuMirror.push_back(element);
        }

        D3D11_BUFFER_DESC desc{};
        desc.Usage = D3D11_USAGE_DYNAMIC;
        desc.ByteWidth = static_cast<UINT>(sizeof(ElementType) * count);
        desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        m_buffer = device->CreateBuffer(desc, elements.GetData());
    }

    virtual void Activate(Device* device, uint32_t offset = 0) override {
        CallAndRethrowM + [&] {
            auto pBuffer = m_buffer.Get();
            auto pContext = device->GetContext();
            UINT stride = sizeof(ElementType);
            pContext->IASetVertexBuffers(0, 1, &pBuffer, &stride, &offset);
            pContext->IASetPrimitiveTopology(m_topology);
        };
    }

    edt::DenseArrayView<ElementType> MakeViewCPU() {
        return CallAndRethrowM + [&] {
            return edt::DenseArrayView<ElementType>(&m_cpuMirror[0], m_cpuMirror.size());
        };
    }

    d3d_tools::BufferMapper<ElementType> MakeBufferMapper(IntrusivePtr<Device> device, D3D11_MAP map) {
        return d3d_tools::BufferMapper<ElementType>(m_buffer, device->GetContext(), map);
    }

private:
    ComPtr<ID3D11Buffer> m_buffer;
    D3D_PRIMITIVE_TOPOLOGY m_topology;
    std::vector<ElementType> m_cpuMirror;
};

class GraphicsSystem :
    public ISystem,
    public IMainWindowListener,
    public Observable<GraphicsSystem, IGraphicsListener>
{
public:
    struct CreateParams {
        HWND hWnd = nullptr;
        bool debugDevice = false;
        bool noDeviceMultithreading = false;
        int Width = 1280;
        int Height = 720;
    };

    GraphicsSystem(Application* app, CreateParams& params);
    bool Update(IApplication* app) override;
    void OnWindowResize(int w, int h) override;

    void Initialize(CreateParams params);

    template<ShaderType shaderType>
    decltype(auto) CreateShaderFromFile(const char* fileName, const char* entryPoint, ShaderVersion shaderVersion) {
        return CallAndRethrowM + [&]{
            auto resourceSystem = m_app->GetResourceSystem();
            auto fileResource = resourceSystem->GetResource<FileResource>(fileName);
            return m_device->CreateShader<shaderType>(fileResource->GetDataView(), entryPoint, shaderVersion);
        };
    }

private:
    Application* m_app = nullptr;
    
    std::unique_ptr<IGpuBuffer> m_buffers[3];

    struct ShaderInfo {
        void Activate(Device* device) {
            CallAndRethrowM + [&] {
                device->SetShader(vs);
                device->SetShader(ps);
                device->GetContext()->IASetInputLayout(layout.Get());
            };
        }

        ComPtr<ID3D11InputLayout> layout;
        Shader<ShaderType::Vertex> vs;
        Shader<ShaderType::Pixel> ps;
    };
    
    struct RT {
        void Activate(Device* device) {
            device->SetRenderTarget(*rt_rtv, ds_dsv.Get());
        }

        std::unique_ptr<Texture> rt;
        IntrusivePtr<TextureView<ResourceViewType::RenderTarget>> rt_rtv;
        IntrusivePtr<TextureView<ResourceViewType::ShaderResource>> rt_srv;

        std::unique_ptr<Texture> ds;
        IntrusivePtr<TextureView<ResourceViewType::DepthStencil>> ds_dsv;
        IntrusivePtr<TextureView<ResourceViewType::ShaderResource>> ds_srv;
    } m_renderTargets[3];

    ShaderInfo m_drawShader;
    ShaderInfo m_uiShader;
    ComPtr<ID3D11SamplerState> m_sampler;

    bool m_fullscreen = false;
    IntrusivePtr<Device> m_device;
    IntrusivePtr<SwapChain> m_swapchain;
    IntrusivePtr<TextureView<ResourceViewType::RenderTarget>> m_renderTargetView;
};