#include "GraphicsSystem.h"

#include "Keng/Base/Serialization/OpenArchiveJSON.h"
#include "Keng/Base/Serialization/SerializeMandatory.h"
#include "Keng/FileSystem/ReadFileToBuffer.h"
#include "Keng/ResourceSystem/IResourceSystem.h"
#include "Keng/WindowSystem/IWindow.h"
#include "Keng/WindowSystem/IWindowSystem.h"

#include "keng/GPU/DeviceParameters.h"
#include "keng/GPU/Resource/TextureParameters.h"
#include "Resource/Texture/Texture.h"
#include "Keng/Graphics/Resource/IEffect.h"
#include "Resource/Effect/Effect.h"
#include "Resource/ResourceFabricRegisterer.h"

#include "Keng/GPU/ViewportParameters.h"

#include "Keng/GPU/IAnnotation.h"

#include "Device.h"
#include "Resource/Texture/Texture.h"

#include "EverydayTools/Array/ArrayViewVector.h"
#include "EverydayTools/Exception/CheckedCast.h"
#include "EverydayTools/Geom/Vector.h"
#include "Keng/GPU/Resource/IDeviceBuffer.h"

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
                m_api = m_app->FindSystem<gpu::IGPUSystem>();
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

    IDevicePtr GraphicsSystem::GetDevice() {
        return m_device;
    }

    ITexturePtr GraphicsSystem::CreateTexture(const gpu::TextureParameters& params) {
        return m_device->CreateRuntimeTexture(params, *m_resourceSystem);
    }

    void GraphicsSystem::Draw(size_t vertices, size_t offset) {
        m_api->Draw(vertices, offset);
    }

    void GraphicsSystem::SetViewport(const gpu::ViewportParameters& p) {
        m_api->SetViewport(p);
    }

    gpu::IAnnotationPtr GraphicsSystem::CreateAnnotation() {
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
                gpu::IGPUSystem::SystemName()
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
