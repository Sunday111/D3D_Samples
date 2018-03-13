#include "Application.h"
#include "GraphicsSystem.h"
#include "D3D_Tools/Annotation.h"
#include <algorithm>

namespace {
    struct vec3 {
        union {
            float arr[3];
            float x, y, z;
        };
    };

    struct rgba {
        union {
            float arr[4];
            float r, g, b, a;
        };
    };

    struct Vertex {
        vec3 pos;
        rgba col;
    };
}

GraphicsSystem::GraphicsSystem(Application* app, CreateParams& params) :
    m_app(app)
{
    CallAndRethrowM + [&]{
        Initialize(params);

        app->GetWindowSystem()->GetWindow()->Subscribe(this);

        {// Read and compile shaders
            { // Draw shader
                D3D11_INPUT_ELEMENT_DESC elementDesc[]{
                    { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
                    { "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
                };
                m_drawShader.vs = CreateShaderFromFile<ShaderType::Vertex>("Assets/Shaders/Shader.hlsl", "VShader", ShaderVersion::_5_0);
                m_drawShader.ps = CreateShaderFromFile<ShaderType::Pixel>("Assets/Shaders/Shader.hlsl", "PShader", ShaderVersion::_5_0);
                m_drawShader.layout = m_device->CreateInputLayout(elementDesc, 2, m_drawShader.vs.bytecode.Get());
            }
            { // Ui shader
                D3D11_INPUT_ELEMENT_DESC elementDesc[]{
                    { "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT,    0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
                    { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0,  8, D3D11_INPUT_PER_VERTEX_DATA, 0 }
                };
                m_uiShader.vs = CreateShaderFromFile<ShaderType::Vertex>("Assets/Shaders/Ui.hlsl", "VShader", ShaderVersion::_5_0);
                m_uiShader.ps = CreateShaderFromFile<ShaderType::Pixel>("Assets/Shaders/Ui.hlsl", "PShader", ShaderVersion::_5_0);
                m_uiShader.layout = m_device->CreateInputLayout(elementDesc, 2, m_uiShader.vs.bytecode.Get());
            }
        }

        {// Create vertex buffer
            Vertex vertices[] = {
                { -0.50f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f },
                { -0.05f, -0.5,  0.0f, 0.0f, 1.0f, 0.0f, 1.0f },
                { -0.95f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f }
            };

            m_buffer = std::make_unique<GpuBuffer<Vertex>>(m_device, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP, edt::MakeArrayView(vertices));
        }

        // Create textures
        {
			auto& rt = m_renderTarget;
            auto device = m_device->GetDevice();
            rt.rt = std::make_unique<Texture>(device, params.Width, params.Height, TextureFormat::R8_G8_B8_A8_UNORM, TextureFlags::RenderTarget | TextureFlags::ShaderResource);
            rt.rt_rtv = rt.rt->GetView<ResourceViewType::RenderTarget>(device, rt.rt->GetFormat());
            rt.rt_srv = rt.rt->GetView<ResourceViewType::ShaderResource>(device, rt.rt->GetFormat());
            rt.ds = std::make_unique<Texture>(device, params.Width, params.Height, TextureFormat::R24_G8_TYPELESS, TextureFlags::DepthStencil | TextureFlags::ShaderResource);
            rt.ds_dsv = rt.ds->GetView<ResourceViewType::DepthStencil>(device, TextureFormat::D24_UNORM_S8_UINT);
            rt.ds_srv = rt.ds->GetView<ResourceViewType::ShaderResource>(device, TextureFormat::R24_UNORM_X8_TYPELESS);
        }

        {// Create sampler
            CD3D11_SAMPLER_DESC samplerDesc(D3D11_DEFAULT);
            samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
            samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
            m_device->GetDevice()->CreateSamplerState(&samplerDesc, m_sampler.Receive());
        }
    };
}

bool GraphicsSystem::Update(IApplication* iapp) {
    return CallAndRethrowM + [&] {
        return d3d_tools::Annotate(m_device.Get(), L"Frame", [&]() {
            UnusedVar(iapp);

            float clearColor[4]{
                0.0f, 0.2f, 0.4f, 1.0f
            };

            static float angle = 0.f;
            constexpr float delta_angle = 0.001f;
            angle += delta_angle;

			d3d_tools::Annotate(m_device.Get(), L"Move triangle", [&]() {
				auto castedBuffer = static_cast<GpuBuffer<Vertex>*>(m_buffer.get());
				auto mapper = castedBuffer->MakeBufferMapper(m_device, D3D11_MAP_WRITE_DISCARD);
				auto view = castedBuffer->MakeViewCPU();
				for (size_t i = 0; i < view.GetSize(); ++i) {
					view[i].pos.x += 0.003f * std::sin(angle);
					mapper.At(i) = view[i];
				}
            });

			d3d_tools::Annotate(m_device.Get(), L"Draw triangle", [&]() {
				m_device->GetContext()->ClearRenderTargetView(m_renderTarget.rt_rtv->GetView(), clearColor);
				m_device->GetContext()->ClearDepthStencilView(m_renderTarget.ds_dsv->GetView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
				m_renderTarget.Activate(m_device.Get());
				m_drawShader.Activate(m_device.Get());
				m_buffer->Activate(m_device.Get());
				m_device->Draw(3);
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

void GraphicsSystem::OnWindowResize(int w, int h) {
    UnusedVar(w, h);
    OutputDebugStringA("Resize\n");
}

void GraphicsSystem::Initialize(CreateParams params) {
    CallAndRethrowM + [&] {
        {// Initialize device
            Device::CreateParams deviceParams;
            deviceParams.debugDevice = params.debugDevice;
            deviceParams.noDeviceMultithreading = params.noDeviceMultithreading;
            m_device = IntrusivePtr<Device>::MakeInstance(deviceParams);
        }

        {// Initialize swapchain
            m_swapchain = IntrusivePtr<SwapChain>::MakeInstance(m_device->GetDevice(), params.Width, params.Height, params.hWnd);
        }

        {// Create render target view to swapchain backbuffer
            auto backbuffer = m_swapchain->GetBackBuffer();
            m_renderTargetView = IntrusivePtr<TextureView<ResourceViewType::RenderTarget>>::MakeInstance(
                m_device->GetDevice(),
                backbuffer.GetTexture());
        }

        {// Initialize viewport
            D3D11_VIEWPORT viewport[1]{};
            viewport[0].TopLeftX = 0;
            viewport[0].TopLeftY = 0;
            viewport[0].Width = static_cast<float>(params.Width);
            viewport[0].Height = static_cast<float>(params.Height);
            m_device->SetViewports(edt::MakeArrayView(viewport));
        }
    };
}