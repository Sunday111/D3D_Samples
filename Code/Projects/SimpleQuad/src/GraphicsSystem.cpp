#include "GraphicsSystem.h"
#include "EverydayTools/Geom/Vector.h"
#include "D3D_Tools/Annotation.h"
#include "Keng/Graphics/IEffect.h"
#include "Keng/Graphics/Texture.h"
#include "Keng/ResourceSystem/IResourceSystem.h"
#include "Keng/Core/Application.h"
#include "EverydayTools/Array/ArrayViewVector.h"

namespace simple_quad_sample
{
	namespace {
		struct Vertex {
			edt::geom::Vector<float, 4> pos;
			edt::geom::Vector<float, 4> col;
		};
	}

	bool GraphicsSystem::Update() {
		using namespace keng;

		return CallAndRethrowM + [&] {
			return d3d_tools::Annotate(m_device.get(), L"Frame", [&]() {
				float clearColor[4] {
					0.0f, 0.2f, 0.4f, 1.0f
				};

				static float angle = 0.f;
				constexpr float delta_angle = 0.001f;
				angle += delta_angle;

				d3d_tools::Annotate(m_device.get(), L"Move triangle", [&]() {
					auto castedBuffer = static_cast<d3d_tools::CrossDeviceBuffer<Vertex>*>(m_buffer.get());
					castedBuffer->BeginUpdate();
					auto bufferView = castedBuffer->MakeView();
					for (size_t i = 0; i < bufferView.GetSize(); ++i) {
						bufferView[i].pos.rx() += 0.00f * std::sin(angle);
					}
					castedBuffer->EndUpdate();
					castedBuffer->Sync(m_device.get());
				});

				d3d_tools::Annotate(m_device.get(), L"Draw triangle", [&]() {
					m_device->GetContext()->ClearRenderTargetView(m_renderTarget.rt_rtv->GetView(), clearColor);
					m_device->GetContext()->ClearDepthStencilView(m_renderTarget.ds_dsv->GetView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
					m_renderTarget.Activate(m_device.get());
					m_effect->Use(m_device.get());
					m_buffer->GetGpuBuffer()->Activate(m_device.get());
					m_device->Draw(4);
				});

				d3d_tools::Annotate(m_device.get(), L"Copy texture to swap chain texture", [&]() {
					ID3D11Resource* finalRT;
					m_renderTargetView->GetView()->GetResource(&finalRT);
					auto nativeTexture = static_cast<ID3D11Texture2D*>(m_renderTarget.rt->GetNativeInterface());
					m_device->GetContext()->CopyResource(finalRT, nativeTexture);
				});

				m_swapchain->Present();

				return true;
			});
		};
	}

	void GraphicsSystem::Initialize(keng::core::IApplication* app)
	{
		CallAndRethrowM + [&] {
			Base::Initialize(app);
			
			using namespace keng;
			auto resourceSystem = m_app->GetSystem<resource::IResourceSystem>();

			{// Read and compile shaders
                std::string_view effectName = "Assets/Effects/FlatColor.xml";
				m_effect = std::dynamic_pointer_cast<graphics::IEffect>(resourceSystem->GetResource(effectName));
                m_effect->InitDefaultInputLayout(m_device.get());
			}

			{// Create vertex buffer
				Vertex vertices[4];

				//      POSITION               ////         COLOR               
				/////////////////////////////////////////////////////////////////////
				vertices[0].pos.rx() = -0.50f; /**/ vertices[0].col.rx() = 0.0f; /**/
                vertices[0].pos.ry() = -0.50f; /**/ vertices[0].col.ry() = 0.0f; /**/
                vertices[0].pos.rz() = +0.00f; /**/ vertices[0].col.rz() = 1.0f; /**/
                vertices[0].pos.rw() = +1.00f; /**/ vertices[0].col.rw() = 1.0f; /**/
                /////////////////////////////////////////////////////////////////////
				vertices[1].pos.rx() = -0.50f; /**/ vertices[1].col.rx() = 1.0f; /**/
                vertices[1].pos.ry() = +0.50f; /**/ vertices[1].col.ry() = 0.0f; /**/
                vertices[1].pos.rz() = +0.00f; /**/ vertices[1].col.rz() = 0.0f; /**/
                vertices[1].pos.rw() = +1.00f; /**/ vertices[1].col.rw() = 1.0f; /**/
                /////////////////////////////////////////////////////////////////////
				vertices[2].pos.rx() = +0.50f; /**/ vertices[2].col.rx() = 0.0f; /**/
                vertices[2].pos.ry() = -0.50f; /**/ vertices[2].col.ry() = 1.0f; /**/
                vertices[2].pos.rz() = +0.00f; /**/ vertices[2].col.rz() = 0.0f; /**/
                vertices[2].pos.rw() = +1.00f; /**/ vertices[2].col.rw() = 1.0f; /**/
                /////////////////////////////////////////////////////////////////////
				vertices[3].pos.rx() = +0.50f; /**/ vertices[3].col.rx() = 0.0f; /**/
                vertices[3].pos.ry() = +0.50f; /**/ vertices[3].col.ry() = 1.0f; /**/
                vertices[3].pos.rz() = +0.00f; /**/ vertices[3].col.rz() = 0.0f; /**/
                vertices[3].pos.rw() = +1.00f; /**/ vertices[3].col.rw() = 1.0f; /**/
                /////////////////////////////////////////////////////////////////////

				m_buffer = std::make_unique<d3d_tools::CrossDeviceBuffer<Vertex>>(
					m_device.get(),
					D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
					edt::MakeArrayView(vertices));
			}
		};
	}

}
