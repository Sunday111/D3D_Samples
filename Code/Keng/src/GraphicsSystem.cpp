#include "Keng/Systems/GraphicsSystem/GraphicsSystem.h"
#include "D3D_Tools/Annotation.h"
#include "Keng/Systems/WindowSystem/WindowSystem.h"
#include <algorithm>

#include "Keng/Rendering/Effect.h"
#include "Keng/Rendering/Shader.h"
#include "Keng/Rendering/ShaderTemplate.h"
#include "Keng/Rendering/Texture.h"

#include "EverydayTools/Geom/Vector.h"
#include "Keng/Application.h"

namespace keng
{
	namespace {
		struct Vertex {
			edt::geom::Vector<float, 3> pos;
			edt::geom::Vector<float, 4> col;
			edt::geom::Vector<float, 2> tex;
		};
	}

	void GraphicsSystem::RT::Activate(Device* device) {
		device->SetRenderTarget(*rt_rtv, ds_dsv.Get());
	}

	GraphicsSystem::GraphicsSystem()
	{
		m_dependencies.push_back(ResourceSystem::GetGUID());
		m_dependencies.push_back(WindowSystem::GetGUID());
	}

	const char* GraphicsSystem::GetGUID()
	{
		return "E6080ACE-E91E-4693-AFA5-5B6A0BB29A41";
	}

	void GraphicsSystem::OnWindowResize(int w, int h) {
		UnusedVar(w, h);
		OutputDebugStringA("Resize\n");
	}

	void GraphicsSystem::Initialize(const CreateParams& params) {
		CallAndRethrowM + [&] {
			{// Initialize device
				Device::CreateParams deviceParams;
				deviceParams.debugDevice = params.debugDevice;
				deviceParams.noDeviceMultithreading = params.noDeviceMultithreading;
				m_device = IntrusivePtr<Device>::MakeInstance(deviceParams);
			}

			auto wndSystem = m_app->GetSystem<WindowSystem>();
			auto window = wndSystem->GetWindow();
			uint32_t w, h;
			window->GetWindowClientSize(&w, &h);

			{// Initialize swapchain
				m_swapchain = IntrusivePtr<SwapChain>::MakeInstance(m_device->GetDevice(), w, h, window->GetHandle());
			}

			{// Create render target view to swapchain backbuffer
				auto backbuffer = m_swapchain->GetBackBuffer();
				m_renderTargetView = IntrusivePtr<TextureView<ResourceViewType::RenderTarget>>::MakeInstance(
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

			m_app->GetSystem<WindowSystem>()->GetWindow()->Subscribe(this);;
			auto resourceSystem = m_app->GetSystem<ResourceSystem>();
			resourceSystem->RegisterResourceFabric(IntrusivePtr<ShaderTemplateFabric>::MakeInstance());
			resourceSystem->RegisterResourceFabric(IntrusivePtr<ShaderFabric>::MakeInstance(m_device));
			resourceSystem->RegisterResourceFabric(IntrusivePtr<EffectFabric>::MakeInstance());
			resourceSystem->RegisterResourceFabric(IntrusivePtr<TextureFabric>::MakeInstance(m_device));

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

	void GraphicsSystem::Initialize(IApplication* app)
	{
		m_app = dynamic_cast<Application*>(app);
		edt::ThrowIfFailed(m_app != nullptr, "Failed to cast IApplication to keng::Application");

		
		CreateParams params{};
		
#ifdef _DEBUG
		params.noDeviceMultithreading = true;
		params.debugDevice = true;
#endif
		
		Initialize(params);
	}

	void GraphicsSystem::ShaderInfo::Activate(Device* device)
	{
		CallAndRethrowM + [&] {
			device->SetShader(effect->vs->m_impl);
			device->SetShader(effect->fs->m_impl);
			device->GetContext()->IASetInputLayout(layout.Get());
		};
	}
}
