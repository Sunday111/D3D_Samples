#include "Keng/Graphics/GraphicsSystem.h"

#include "Keng/Base/Serialization/OpenArchiveJSON.h"
#include "Keng/Base/Serialization/SerializeMandatory.h"
#include "Keng/Base/Serialization/ReadFileToBuffer.h"
#include "Keng/Core/Application.h"
#include "Keng/ResourceSystem/IResourceSystem.h"
#include "Keng/WindowSystem/IWindow.h"
#include "Keng/WindowSystem/IWindowSystem.h"

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

#include "Sampler.h"

#include "EverydayTools/Geom/Vector.h"
#include "DeviceBuffer.h"

#include <algorithm>

#include "yasli/JSONIArchive.h"

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
                    auto buffer = ReadFileToBuffer("Configs/graphics_system.json");
                    yasli::JSONIArchive ar;
                    OpenArchiveJSON(buffer, ar);
                    ar(file);
                } catch (...) {
                }

                return file.params;
            };
        }
    }

    GraphicsSystem::GraphicsSystem() {
        m_dependencies.push_back(resource::IResourceSystem::GetGUID());
        m_dependencies.push_back(window_system::IWindowSystem::GetGUID());
    }

    GraphicsSystem::~GraphicsSystem() = default;

    const char* GraphicsSystem::GetGUID() {
        return "E6080ACE-E91E-4693-AFA5-5B6A0BB29A41";
    }

    void GraphicsSystem::Initialize(core::IApplication* app) {
        CallAndRethrowM + [&] {
            m_app = dynamic_cast<core::Application*>(app);
            edt::ThrowIfFailed(m_app != nullptr, "Failed to cast IApplication to keng::Application");
            auto params = ReadDefaultParams();

            {// Register resource fabrics
                auto resourceSystem = m_app->GetSystem<resource::IResourceSystem>();
                ResourceFabricRegisterer fabricRegisterer;
                fabricRegisterer.Register(resourceSystem);
            }

            {// Initialize device
                Device::CreateParams deviceParams;
                deviceParams.debugDevice = params.debugDevice;
                deviceParams.noDeviceMultithreading = !params.deviceMultithreading;
                m_device = DevicePtr::MakeInstance(deviceParams);
            }
        };
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
        auto resourceSystem = m_app->GetSystem<resource::IResourceSystem>();
        resourceSystem->AddRuntimeResource(tex, m_device);
        return tex;
    }

    ISamplerPtr GraphicsSystem::CreateSampler(const SamplerParameters& params) {
        return SamplerPtr::MakeInstance(*m_device, params);
    }

    bool GraphicsSystem::ForEachSystemDependency(bool(*pfn)(const char* systemGUID, void* context), void* context) {
        return CallAndRethrowM + [&]() -> bool {
            for (auto& guid : m_dependencies) {
                if (pfn(guid.data(), context)) {
                    return true;
                }
            }

            return false;
        };
    }

    const char* GraphicsSystem::GetSystemGUID() {
        return GetGUID();
    }
}
