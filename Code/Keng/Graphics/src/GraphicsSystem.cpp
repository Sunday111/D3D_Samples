#include "GraphicsSystem.h"

#include "Keng/Base/Serialization/OpenArchiveJSON.h"
#include "Keng/Base/Serialization/SerializeMandatory.h"
#include "Keng/FileSystem/ReadFileToBuffer.h"
#include "Keng/ResourceSystem/IResourceSystem.h"
#include "Keng/WindowSystem/IWindow.h"
#include "Keng/WindowSystem/IWindowSystem.h"

#include "keng/Graphics/DeviceParameters.h"
#include "keng/Graphics/Resource/TextureParameters.h"
#include "Resource/Texture/Texture.h"
#include "Keng/Graphics/Resource/IEffect.h"
#include "Resource/Effect/Effect.h"
#include "Resource/ResourceFabricRegisterer.h"

#include "Keng/Graphics/RenderTarget/WindowRenderTargetParameters.h"
#include "RenderTarget/WindowRenderTarget.h"
#include "Keng/Graphics/RenderTarget/TextureRenderTargetParameters.h"
#include "RenderTarget/TextureRenderTarget.h"
#include "Keng/Graphics/RenderTarget/DepthStencilParameters.h"
#include "RenderTarget/DepthStencil.h"
#include "Keng/Graphics/RenderTarget/DepthStencilParameters.h"
#include "RenderTarget/DepthStencil.h"
#include "Keng/Graphics/RenderTarget/SwapChainParameters.h"
#include "RenderTarget/SwapChain.h"
#include "Keng/Graphics/ViewportParameters.h"

#include "Annotation.h"

#include "Sampler.h"

#include "EverydayTools/Array/ArrayViewVector.h"
#include "EverydayTools/Exception/CheckedCast.h"
#include "EverydayTools/Geom/Vector.h"
#include "DeviceBuffer.h"

#include <algorithm>

#include "yasli/JSONIArchive.h"
#include "D3D_11/EnumConverter.h"
#include "Keng/Core/IApplication.h"

namespace keng::graphics
{
    namespace
    {
        struct Vertex
        {
            edt::geom::Vector<float, 3> pos;
            edt::geom::Vector<float, 4> col;
            edt::geom::Vector<float, 2> tex;
        };

        struct SystemParams
        {
            void serialize(Archive& ar) {
                ar(debugDevice, "debug_device");
                ar(deviceMultithreading, "device_multithreading");
            }
            bool debugDevice = false;
            bool deviceMultithreading = false;
        };

        SystemParams ReadDefaultParams() {
            return CallAndRethrowM + [&] {
                struct ConfigFile
                {
                    void serialize(Archive& ar) { ar(params, "graphics_system"); }
                    SystemParams params;
                };

                ConfigFile file;

#ifdef _DEBUG
                file.params.deviceMultithreading = true;
                file.params.debugDevice = true;
#endif

                try {
                    auto filename = "Configs/graphics_system.json";
                    using FileView = edt::DenseArrayView<const uint8_t>;
                    auto onFileRead = [&](FileView fileView) {
                        yasli::JSONIArchive ar;
                        OpenArchiveJSON(fileView, ar);
                        ar(file);
                    };
                    edt::Delegate<void(FileView)> delegate;
                    delegate.Bind(onFileRead);
                    filesystem::HandleFileData(filename, delegate);
                } catch (...) {
                }

                return file.params;
            };
        }
    }

    GraphicsSystem::GraphicsSystem() = default;

    GraphicsSystem::~GraphicsSystem() = default;

    void GraphicsSystem::Initialize(const core::IApplicationPtr& app) {
        CallAndRethrowM + [&] {
            m_app = app;

            {// Find systems
                m_resourceSystem = m_app->FindSystem<resource::IResourceSystem>();
                m_api = m_app->FindSystem<graphics_api::IGraphicsAPISystem>();
            }

            auto params = ReadDefaultParams();

            {// Register resource fabrics
                ResourceFabricRegisterer fabricRegisterer;
                fabricRegisterer.Register(m_resourceSystem);
            }

            {// Initialize device
                DeviceParameters deviceParams;
                deviceParams.debugDevice = params.debugDevice;
                deviceParams.noDeviceMultithreading = !params.deviceMultithreading;
                m_device = DevicePtr::MakeInstance(deviceParams);
            }
        };
    }

    bool GraphicsSystem::Update() {
        return true;
    }

    void GraphicsSystem::Shutdown() {
    }

    IWindowRenderTargetPtr GraphicsSystem::CreateWindowRenderTarget(const WindowRenderTargetParameters& params) {
        return WindowRenderTargetPtr::MakeInstance(*m_device, params);
    }

    ITextureRenderTargetPtr GraphicsSystem::CreateTextureRenderTarget(const TextureRenderTargetParameters& params) {
        return TextureRenderTargetPtr::MakeInstance(*m_device, params);
    }

    IDepthStencilPtr GraphicsSystem::CreateDepthStencil(const DepthStencilParameters& params) {
        return DepthStencilPtr::MakeInstance(*m_device, params);
    }

    core::Ptr<IDevice> GraphicsSystem::GetDevice() {
        return m_device;
    }

    IDeviceBufferPtr GraphicsSystem::CreateDeviceBuffer(const DeviceBufferParams& params, edt::DenseArrayView<const uint8_t> data) {
        return CallAndRethrowM + [&] {
            return DeviceBufferPtr::MakeInstance(*m_device, params, data);
        };
    }

    ISwapChainPtr GraphicsSystem::CreateSwapChain(const SwapChainParameters& params) {
        return SwapChainPtr::MakeInstance(*m_device, params);
    }

    ITexturePtr GraphicsSystem::CreateTexture(const TextureParameters& params) {
        auto tex = TexturePtr::MakeInstance(*m_device, params);
        m_resourceSystem->AddRuntimeResource(tex, m_device);
        return tex;
    }

    ISamplerPtr GraphicsSystem::CreateSampler(const SamplerParameters& params) {
        return SamplerPtr::MakeInstance(*m_device, params);
    }

    void GraphicsSystem::SetTopology(PrimitiveTopology topo) {
        m_device->GetContext()->IASetPrimitiveTopology(d3d::ConvertTopology(topo));
    }

    void GraphicsSystem::Draw(size_t vertices, size_t offset) {
        m_device->GetContext()->Draw(
            edt::CheckedCast<uint32_t>(vertices),
            edt::CheckedCast<uint32_t>(offset));
    }

    void GraphicsSystem::SetViewport(const ViewportParameters& p) {
        D3D11_VIEWPORT v[1]{};
        v[0].TopLeftX = p.Position.x();
        v[0].TopLeftY = p.Position.y();
        v[0].Width = p.Size.x();
        v[0].Height = p.Size.y();
        m_device->SetViewports(edt::MakeArrayView(v));
    }

    IAnnotationPtr GraphicsSystem::CreateAnnotation() {
        return AnnotationPtr::MakeInstance(*m_device);
    }

    const char* GraphicsSystem::GetSystemName() const {
        return SystemName();
    }

    bool GraphicsSystem::ForEachDependency(const edt::Delegate<bool(const char*)>& delegate) const {
        return CallAndRethrowM + [&]() -> bool {
            std::string_view dependencies[] =
            {
                resource::IResourceSystem::SystemName(),
                window_system::IWindowSystem::SystemName(),
                graphics_api::IGraphicsAPISystem::SystemName()
            };

            for (auto& systemName : dependencies) {
                if (delegate.Invoke(systemName.data())) {
                    return true;
                }
            }

            return false;
        };
    }
}
