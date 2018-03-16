#include "GraphicsSystem.h"
#include "EverydayTools/Geom/Vector.h"
#include "D3D_Tools/Annotation.h"
#include "Keng/Rendering/Shader.h"
#include "Keng/Rendering/Effect.h"
#include "Keng/Rendering/Texture.h"
#include "ResourceSystem/ResourceSystem.h"
#include "SystemsApp/BaseApplication.h"

namespace simple_quad_sample
{
	namespace {
		struct Vertex {
			edt::geom::Vector<float, 3> pos;
			edt::geom::Vector<float, 4> col;
			edt::geom::Vector<float, 2> tex;
		};
	}

	bool GraphicsSystem::Update() {
		using namespace keng;

		return CallAndRethrowM + [&] {
			return d3d_tools::Annotate(m_device.Get(), L"Frame", [&]() {
				float clearColor[4] {
					0.0f, 0.2f, 0.4f, 1.0f
				};

				static float angle = 0.f;
				constexpr float delta_angle = 0.001f;
				angle += delta_angle;

				d3d_tools::Annotate(m_device.Get(), L"Move triangle", [&]() {
					auto castedBuffer = static_cast<d3d_tools::CrossDeviceBuffer<Vertex>*>(m_buffer.get());
					castedBuffer->BeginUpdate();
					auto bufferView = castedBuffer->MakeView();
					for (size_t i = 0; i < bufferView.GetSize(); ++i) {
						bufferView[i].pos.rx() += 0.00f * std::sin(angle);
					}
					castedBuffer->EndUpdate();
					castedBuffer->Sync(m_device.Get());
				});

				d3d_tools::Annotate(m_device.Get(), L"Draw triangle", [&]() {
					m_device->GetContext()->ClearRenderTargetView(m_renderTarget.rt_rtv->GetView(), clearColor);
					m_device->GetContext()->ClearDepthStencilView(m_renderTarget.ds_dsv->GetView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
					m_renderTarget.Activate(m_device.Get());
					m_drawShader.Activate(m_device.Get());
					m_buffer->GetGpuBuffer()->Activate(m_device.Get());
					m_device->Draw(4);
				});

				d3d_tools::Annotate(m_device.Get(), L"Copy texture to swap chain texture", [&]() {
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

	void GraphicsSystem::Initialize(const CreateParams& params, IApplication* app)
	{
		CallAndRethrowM + [&] {
			Base::Initialize(params, app);
			
			using namespace keng;

			auto resourceSystem = m_app->GetSystem<ResourceSystem>();

			{// Read and compile shaders
				D3D11_INPUT_ELEMENT_DESC elementDesc[] {
					{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
					{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
					{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
				};
				m_drawShader.effect = std::dynamic_pointer_cast<Effect>(resourceSystem->GetResource("Assets/Effects/DefaultEffect.xml"));
				m_drawShader.layout = m_device->CreateInputLayout(elementDesc, 3, m_drawShader.effect->vs->m_impl.bytecode.Get());
			}

			{// Create vertex buffer
				Vertex vertices[4];

				//      POSITION               ////         COLOR                ////         TEXTURE COORDS     
				vertices[0].pos.rx() = -0.50f; /**/ vertices[0].col.rx() = 0.0f; /**/ vertices[0].tex.rx() = 0.0f;
				vertices[0].pos.ry() = -0.50f; /**/ vertices[0].col.ry() = 0.0f; /**/ vertices[0].tex.ry() = 0.0f;
				vertices[0].pos.rz() = 0.00f;  /**/ vertices[0].col.rz() = 1.0f; /**/
				                               /**/ vertices[0].col.rw() = 1.0f; /**/
				                               /**/                              /**/
				vertices[1].pos.rx() = -0.50f; /**/ vertices[1].col.rx() = 1.0f; /**/ vertices[1].tex.rx() = 0.0f;
				vertices[1].pos.ry() = 0.50f;  /**/ vertices[1].col.ry() = 0.0f; /**/ vertices[1].tex.ry() = 1.0f;
				vertices[1].pos.rz() = 0.00f;  /**/ vertices[1].col.rz() = 0.0f; /**/
				                               /**/ vertices[1].col.rw() = 1.0f; /**/
				                               /**/                              /**/
				vertices[2].pos.rx() = 0.50f;  /**/ vertices[2].col.rx() = 0.0f; /**/ vertices[2].tex.rx() = 1.0f;
				vertices[2].pos.ry() = -0.50f; /**/ vertices[2].col.ry() = 1.0f; /**/ vertices[2].tex.ry() = 0.0f;
				vertices[2].pos.rz() = 0.00f;  /**/ vertices[2].col.rz() = 0.0f; /**/
				                               /**/ vertices[2].col.rw() = 1.0f; /**/
				                               /**/                              /**/
				vertices[3].pos.rx() = 0.50f;  /**/ vertices[3].col.rx() = 0.0f; /**/ vertices[3].tex.rx() = 1.0f;
				vertices[3].pos.ry() = 0.50f;  /**/ vertices[3].col.ry() = 1.0f; /**/ vertices[3].tex.ry() = 1.0f;
				vertices[3].pos.rz() = 0.00f;  /**/ vertices[3].col.rz() = 0.0f; /**/
				                               /**/ vertices[3].col.rw() = 1.0f; /**/

				m_buffer = std::make_unique<d3d_tools::CrossDeviceBuffer<Vertex>>(
					m_device.Get(),
					D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
					edt::MakeArrayView(vertices));
			}
		};
	}

}