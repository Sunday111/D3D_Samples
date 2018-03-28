#include "Keng/Graphics/GraphicsSystem.h"

#include "D3D_Tools/Annotation.h"

#include "Keng/Core/Application.h"
#include "Keng/ResourceSystem/IResourceSystem.h"
#include "Keng/WindowSystem/IWindow.h"
#include "Keng/WindowSystem/IWindowSystem.h"

#include "Keng/Graphics/Resource/IEffect.h"
#include "Keng/Graphics/Resource/Texture.h"
#include "RenderTarget/WindowRenderTarget.h"
#include "RenderTarget/TextureRenderTarget.h"
#include "Resource/Effect/Effect.h"
#include "Resource/ResourceFabricRegisterer.h"
#include "RenderTarget/WindowRenderTarget.h"

#include "Keng/Graphics/RenderTarget/TextureRenderTargetParameters.h"
#include "Keng/Graphics/RenderTarget/WindowRenderTargetParameters.h"
#include "Keng/Graphics/RenderTarget/DepthStencilParameters.h"

#include "EverydayTools/Geom/Vector.h"
#include "DeviceBuffer.h"
#include "SwapChain.h"
#include "RenderTarget/DepthStencil.h"

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

    void GraphicsSystem::RT::Activate(Device* device) {
        auto deviceImpl = device->GetDevice();
        auto rtv = rt->GetView<ResourceViewType::RenderTarget>(deviceImpl, rt->GetFormat());
        auto dsv = ds->GetView<ResourceViewType::DepthStencil>(deviceImpl, TextureFormat::D24_UNORM_S8_UINT).Get();
        device->SetRenderTarget(*rtv, dsv);
    }

    void GraphicsSystem::RT::Clear(Device* device, const float(&color)[4]) {
        auto deviceImpl = device->GetDevice();
        auto rtv = rt->GetView<ResourceViewType::RenderTarget>(deviceImpl, rt->GetFormat());
        auto dsv = ds->GetView<ResourceViewType::DepthStencil>(deviceImpl, TextureFormat::D24_UNORM_S8_UINT).Get();
        device->GetContext()->ClearRenderTargetView(rtv->GetView(), color);
        device->GetContext()->ClearDepthStencilView(dsv->GetView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
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

            {
                ResourceFabricRegisterer fabricRegisterer(this);
                fabricRegisterer.Register(resourceSystem);
            }

            // Create render target
            {
                auto& rt = m_renderTarget;
                auto device = m_device->GetDevice();
                rt.rt = core::Ptr<Texture>::MakeInstance(device, w, h, TextureFormat::R8_G8_B8_A8_UNORM, TextureFlags::RenderTarget | TextureFlags::ShaderResource);
                rt.ds = core::Ptr<Texture>::MakeInstance(device, w, h, TextureFormat::R24_G8_TYPELESS, TextureFlags::DepthStencil | TextureFlags::ShaderResource);
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
