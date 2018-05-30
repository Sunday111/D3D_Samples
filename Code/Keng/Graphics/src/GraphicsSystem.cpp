#include "GraphicsSystem.h"

#include "Keng/Base/Serialization/OpenArchiveJSON.h"
#include "Keng/Base/Serialization/SerializeMandatory.h"
#include "Keng/FileSystem/ReadFileToBuffer.h"
#include "Keng/ResourceSystem/IResourceSystem.h"
#include "Keng/WindowSystem/IWindow.h"
#include "Keng/WindowSystem/IWindowSystem.h"

#include "keng/GraphicsAPI/DeviceParameters.h"
#include "keng/GraphicsAPI/Resource/TextureParameters.h"
#include "Resource/Texture/Texture.h"
#include "Keng/Graphics/Resource/IEffect.h"
#include "Resource/Effect/Effect.h"
#include "Resource/ResourceFabricRegisterer.h"

#include "Keng/GraphicsAPI/RenderTarget/WindowRenderTargetParameters.h"
#include "Keng/GraphicsAPI/RenderTarget/IWindowRenderTarget.h"
#include "Keng/GraphicsAPI/RenderTarget/TextureRenderTargetParameters.h"
#include "Keng/GraphicsAPI/RenderTarget/ITextureRenderTarget.h"
#include "Keng/GraphicsAPI/RenderTarget/DepthStencilParameters.h"
#include "Keng/GraphicsAPI/RenderTarget/IDepthStencil.h"
#include "Keng/GraphicsAPI/RenderTarget/DepthStencilParameters.h"
#include "Keng/GraphicsAPI/RenderTarget/IDepthStencil.h"
#include "Keng/GraphicsAPI/RenderTarget/SwapChainParameters.h"
#include "Keng/GraphicsAPI/RenderTarget/ISwapChain.h"
#include "Keng/GraphicsAPI/ViewportParameters.h"

#include "Keng/GraphicsAPI/IAnnotation.h"

#include "Keng/GraphicsAPI/ISampler.h"

#include "Device.h"
#include "Resource/Texture/Texture.h"

#include "EverydayTools/Array/ArrayViewVector.h"
#include "EverydayTools/Exception/CheckedCast.h"
#include "EverydayTools/Geom/Vector.h"
#include "Keng/GraphicsAPI/IDeviceBuffer.h"

#include <algorithm>

#include "yasli/JSONIArchive.h"
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

            auto apiDevice = m_api->GetDevice();
            m_device = DevicePtr::MakeInstance(*apiDevice);
        };
    }

    bool GraphicsSystem::Update() {
        return true;
    }

    void GraphicsSystem::Shutdown() {
    }

    graphics_api::IWindowRenderTargetPtr GraphicsSystem::CreateWindowRenderTarget(const graphics_api::WindowRenderTargetParameters& params) {
        return m_api->CreateWindowRenderTarget(params);
    }

    graphics_api::ITextureRenderTargetPtr GraphicsSystem::CreateTextureRenderTarget(const graphics_api::TextureRenderTargetParameters& params) {
        return m_api->CreateTextureRenderTarget(params);
    }

    graphics_api::IDepthStencilPtr GraphicsSystem::CreateDepthStencil(const graphics_api::DepthStencilParameters& params) {
        return m_api->CreateDepthStencil(params);
    }

    IDevicePtr GraphicsSystem::GetDevice() {
        return m_device;
    }

    graphics_api::IDeviceBufferPtr GraphicsSystem::CreateDeviceBuffer(const graphics_api::DeviceBufferParameters& params, edt::DenseArrayView<const uint8_t> data) {
        return m_api->CreateDeviceBuffer(params, data);
    }

    graphics_api::ISwapChainPtr GraphicsSystem::CreateSwapChain(const graphics_api::SwapChainParameters& params) {
        return m_api->CreateSwapChain(params);
    }

    ITexturePtr GraphicsSystem::CreateTexture(const graphics_api::TextureParameters& params) {
        return m_device->CreateRuntimeTexture(params, *m_resourceSystem);
    }

    graphics_api::ISamplerPtr GraphicsSystem::CreateSampler(const graphics_api::SamplerParameters& params) {
        return m_api->CreateSampler(params);
    }

    void GraphicsSystem::SetTopology(graphics_api::PrimitiveTopology topo) {
        m_api->SetTopology(topo);
    }

    void GraphicsSystem::Draw(size_t vertices, size_t offset) {
        m_api->Draw(vertices, offset);
    }

    void GraphicsSystem::SetViewport(const graphics_api::ViewportParameters& p) {
        m_api->SetViewport(p);
    }

    graphics_api::IAnnotationPtr GraphicsSystem::CreateAnnotation() {
        return m_api->CreateAnnotation();
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
