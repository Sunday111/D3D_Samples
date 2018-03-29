#include "Keng/Graphics/GraphicsSystem.h"

#include "Keng/Core/Application.h"
#include "Keng/ResourceSystem/IResourceSystem.h"
#include "Keng/WindowSystem/IWindow.h"
#include "Keng/WindowSystem/IWindowSystem.h"

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

#include "EverydayTools/Geom/Vector.h"
#include "DeviceBuffer.h"
#include "SwapChain.h"

#include <algorithm>
#include "Xml.h"

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
            bool debugDevice = false;
            bool noDeviceMultithreading = false;
        };

        SystemParams ReadDefaultParams() {
            return CallAndRethrowM + [&] {
                SystemParams params;

#ifdef _DEBUG
                params.noDeviceMultithreading = true;
                params.debugDevice = true;
#endif

                try {
                    XmlDocument configDoc("Configs/graphics_system.xml");
                    auto systemNode = configDoc.GetFirstNode("graphics_system");

                    if (auto node = systemNode->FindFirstNode("device_multithreading")) {
                        auto node_text = node->GetValue();
                        params.noDeviceMultithreading = !static_cast<bool>(std::stoi(node_text.data()));
                    }

                    if (auto node = systemNode->FindFirstNode("debug_device")) {
                        auto node_text = node->GetValue();
                        params.debugDevice = static_cast<bool>(std::stoi(node_text.data()));
                    }
                } catch (...) {
                }

                return params;
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

    void GraphicsSystem::OnWindowResize(int w, int h) {
        UnusedVar(w, h);
        OutputDebugStringA("Resize\n");
    }

    void GraphicsSystem::Initialize(core::IApplication* app) {
        CallAndRethrowM + [&] {
            m_app = dynamic_cast<core::Application*>(app);
            edt::ThrowIfFailed(m_app != nullptr, "Failed to cast IApplication to keng::Application");
            auto params = ReadDefaultParams();

            {// Initialize device
                Device::CreateParams deviceParams;
                deviceParams.debugDevice = params.debugDevice;
                deviceParams.noDeviceMultithreading = params.noDeviceMultithreading;
                m_device = core::Ptr<Device>::MakeInstance(deviceParams);
            }
            auto wndSystem = m_app->GetSystem<window_system::IWindowSystem>();
            auto window = wndSystem->GetWindow();
            uint32_t w, h;
            window->GetClientSize(&w, &h);

            {// Initialize swapchain
                WindowRenderTargetParameters window_rt_params;
                window_rt_params.format = d3d_tools::TextureFormat::R8_G8_B8_A8_UNORM;
                window_rt_params.window = window;
                m_windowRT = CreateWindowRenderTarget(window_rt_params);
            }

            {// Initialize viewport
                D3D11_VIEWPORT viewport[1]{};
                viewport[0].TopLeftX = 0;
                viewport[0].TopLeftY = 0;
                viewport[0].Width = static_cast<float>(w);
                viewport[0].Height = static_cast<float>(h);
                m_device->SetViewports(edt::MakeArrayView(viewport));
            }

            m_app->GetSystem<window_system::IWindowSystem>()->GetWindow()->Subscribe(this);;
            auto resourceSystem = m_app->GetSystem<resource::IResourceSystem>();

            {// Register resource fabrics
                ResourceFabricRegisterer fabricRegisterer(this);
                fabricRegisterer.Register(resourceSystem);
            }

            {// Create render target
                TextureRenderTargetParameters texture_rt_params{};
                texture_rt_params.renderTarget = core::Ptr<Texture>::MakeInstance(*m_device, w, h, FragmentFormat::R8_G8_B8_A8_UNORM, TextureFlags::RenderTarget | TextureFlags::ShaderResource);
                m_textureRT = core::Ptr<TextureRenderTarget>::MakeInstance(*m_device, texture_rt_params);
            }
            
            {// Create depth stencil
                DepthStencilParameters depthStencilParams{};
                depthStencilParams.format = FragmentFormat::D24_UNORM_S8_UINT;
                depthStencilParams.texture = core::Ptr<Texture>::MakeInstance(*m_device, w, h, FragmentFormat::R24_G8_TYPELESS, TextureFlags::DepthStencil | TextureFlags::ShaderResource);
                m_depthStencil = core::Ptr<DepthStencil>::MakeInstance(*m_device, depthStencilParams);
            }
        };
    }

    core::Ptr<IWindowRenderTarget> GraphicsSystem::CreateWindowRenderTarget(const WindowRenderTargetParameters& params) {
        return core::Ptr<WindowRenderTarget>::MakeInstance(*m_device, params);
    }

    core::Ptr<ITextureRenderTarget> GraphicsSystem::CreateTextureRenderTarget(const TextureRenderTargetParameters& params) {
        return core::Ptr<TextureRenderTarget>::MakeInstance(*m_device, params);
    }

    core::Ptr<IDepthStencil> GraphicsSystem::CreateDepthStencil(const DepthStencilParameters& params) {
        return core::Ptr<DepthStencil>::MakeInstance(*m_device, params);
    }

    core::Ptr<IWindowRenderTarget> GraphicsSystem::GetWindowRenderTarget() {
        return m_windowRT;
    }

    core::Ptr<IDeviceBuffer> GraphicsSystem::CreateDeviceBuffer(const DeviceBufferParams& params, edt::DenseArrayView<const uint8_t> data) {
        return CallAndRethrowM + [&] {
            return core::Ptr<DeviceBuffer>::MakeInstance(*m_device, params, data);
        };
    }

    core::Ptr<ISwapChain> GraphicsSystem::CreateSwapChain(const SwapChainParameters& params) {
        return core::Ptr<SwapChain>::MakeInstance(*m_device, params);
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
