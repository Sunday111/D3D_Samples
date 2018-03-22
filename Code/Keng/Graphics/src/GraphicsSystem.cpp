#include "Keng/Graphics/GraphicsSystem.h"

#include "D3D_Tools/Annotation.h"

#include "Keng/Core/Application.h"
#include "Keng/ResourceSystem/IResourceSystem.h"
#include "Keng/WindowSystem/IWindowSystem.h"

#include "Keng/Graphics/Resource/IEffect.h"
#include "Keng/Graphics/Resource/Texture.h"
#include "Resource/Effect/Effect.h"
#include "Resource/ResourceFabricRegisterer.h"

#include "EverydayTools/Geom/Vector.h"

#include <algorithm>
#include "Xml.h"

namespace keng::graphics
{
	namespace {
		struct Vertex {
			edt::geom::Vector<float, 3> pos;
			edt::geom::Vector<float, 4> col;
			edt::geom::Vector<float, 2> tex;
		};
	}

    GraphicsSystem::SystemParams GraphicsSystem::ReadDefaultParams() {
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
            }
            catch (...)
            {
            }

            return params;
        };
    }

	void GraphicsSystem::RT::Activate(Device* device) {
		device->SetRenderTarget(*rt_rtv, ds_dsv.get());
	}

	GraphicsSystem::GraphicsSystem()
	{
		m_dependencies.push_back(resource::IResourceSystem::GetGUID());
		m_dependencies.push_back(window_system::IWindowSystem::GetGUID());
	}

	const char* GraphicsSystem::GetGUID()
	{
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
				m_device = std::make_shared<Device>(deviceParams);
			}

			auto wndSystem = m_app->GetSystem<window_system::IWindowSystem>();
			auto window = wndSystem->GetWindow();
			uint32_t w, h;
			window->GetClientSize(&w, &h);

			{// Initialize swapchain
				m_swapchain = std::make_shared<SwapChain>(m_device->GetDevice(), w, h, (HWND)window->GetNativeHandle());
			}

			{// Create render target view to swapchain backbuffer
				auto backbuffer = m_swapchain->GetBackBuffer();
				m_renderTargetView = std::make_shared<TextureView<ResourceViewType::RenderTarget>>(
					m_device->GetDevice(),
					backbuffer.GetTexture());
			}

			{// Initialize viewport
				D3D11_VIEWPORT viewport[1] {};
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
				rt.rt = std::make_unique<Texture>(device, w, h, TextureFormat::R8_G8_B8_A8_UNORM, TextureFlags::RenderTarget | TextureFlags::ShaderResource);
				rt.rt_rtv = rt.rt->GetView<ResourceViewType::RenderTarget>(device, rt.rt->GetFormat());
				rt.rt_srv = rt.rt->GetView<ResourceViewType::ShaderResource>(device, rt.rt->GetFormat());
				rt.ds = std::make_unique<Texture>(device, w, h, TextureFormat::R24_G8_TYPELESS, TextureFlags::DepthStencil | TextureFlags::ShaderResource);
				rt.ds_dsv = rt.ds->GetView<ResourceViewType::DepthStencil>(device, TextureFormat::D24_UNORM_S8_UINT);
				rt.ds_srv = rt.ds->GetView<ResourceViewType::ShaderResource>(device, TextureFormat::R24_UNORM_X8_TYPELESS);
			}
		};
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
