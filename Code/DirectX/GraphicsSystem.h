#pragma once

#include "ISystem.h"
#include "Rendering/Renderer.h"
#include "MainWindow.h"
#include "FileResource.h"
#include "EverydayTools/Observable.h"

class Application;

class IGraphicsListener {
public:
    virtual void BeforeRendering() = 0;
    virtual void AfterRendering() = 0;
    virtual ~IGraphicsListener() = default;
};

class GraphicsSystem :
    public ISystem,
    public IMainWindowListener,
    public Observable<GraphicsSystem, IGraphicsListener>
{
public:
    using CreateParams = Renderer::CreateParams;
    GraphicsSystem(Application* app, Renderer::CreateParams& params);
    bool Update(IApplication* app) override;
    void OnWindowResize(int w, int h) override;

    template<ShaderType shaderType>
    decltype(auto) CreateShaderFromFile(const char* fileName, const char* entryPoint, ShaderVersion shaderVersion) {
        return CallAndRethrowM + [&]{
            auto resourceSystem = m_app->GetResourceSystem();
            auto fileResource = resourceSystem->GetResource<FileResource>(fileName);
            auto device = m_renderer.GetDevice();
            return device->CreateShader<shaderType>(fileResource->GetDataView(), entryPoint, shaderVersion);
        };
    }

    Renderer& GetRenderer() {
        return m_renderer;
    }

private:
    Application* m_app = nullptr;
    Renderer m_renderer;

    struct BufferInfo {
        void Activate(Device* device, uint32_t offset = 0) {
            CallAndRethrowM + [&] {
                auto pBuffer = buffer.Get();
                auto pContext = device->GetContext();
                pContext->IASetVertexBuffers(0, 1, &pBuffer, &stride, &offset);
                pContext->IASetPrimitiveTopology(topo);
            };
        }

        template<typename T>
        edt::DenseArrayView<T> MakeViewCPU() {
            return CallAndRethrowM + [&] {
                if (cpuData.size() % sizeof(T) != 0) {
                    throw std::runtime_error("Trying to map to invalid type!");
                }
                return edt::DenseArrayView<T>((T*)&cpuData[0], cpuData.size() / sizeof(T));
            };
        }

        uint32_t stride;
        ComPtr<ID3D11Buffer> buffer;
        D3D_PRIMITIVE_TOPOLOGY topo;
        std::vector<uint8_t> cpuData;
    } m_buffers[3];

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
            device->SetRenderTarget(rt_rtv.Get(), ds_dsv.Get());
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
};