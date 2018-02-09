#include "Application.h"
#include "GraphicsSystem.h"
#include "IApplication.h"
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

    struct VertexUi {
        float x, y, u, v;
    };
}

GraphicsSystem::GraphicsSystem(Application* app, Renderer::CreateParams& params) :
    m_renderer(params),
    m_app(app)
{
    CallAndRethrowM + [&]{
        app->GetWindowSystem()->GetWindow()->Subscribe(this);
        auto device = m_renderer.GetDevice()->GetDevice();

        {// Read and compile shaders
            { // Draw shader
                D3D11_INPUT_ELEMENT_DESC elementDesc[]{
                    { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
                    { "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
                };
                m_drawShader.vs = CreateShaderFromFile<ShaderType::Vertex>("Assets/Shaders/Shader.hlsl", "VShader", ShaderVersion::_5_0);
                m_drawShader.ps = CreateShaderFromFile<ShaderType::Pixel>("Assets/Shaders/Shader.hlsl", "PShader", ShaderVersion::_5_0);
                m_drawShader.layout = m_renderer.CreateInputLayout(elementDesc, 2, m_drawShader.vs.bytecode.Get());
            }
            { // Ui shader
                D3D11_INPUT_ELEMENT_DESC elementDesc[]{
                    { "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT,    0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
                    { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0,  8, D3D11_INPUT_PER_VERTEX_DATA, 0 }
                };
                m_uiShader.vs = CreateShaderFromFile<ShaderType::Vertex>("Assets/Shaders/Ui.hlsl", "VShader", ShaderVersion::_5_0);
                m_uiShader.ps = CreateShaderFromFile<ShaderType::Pixel>("Assets/Shaders/Ui.hlsl", "PShader", ShaderVersion::_5_0);
                m_uiShader.layout = m_renderer.CreateInputLayout(elementDesc, 2, m_uiShader.vs.bytecode.Get());
            }
        }

        {// Create vertex buffer
            Vertex vertices[] = {
                { -0.50f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f },
                { -0.05f, -0.5,  0.0f, 0.0f, 1.0f, 0.0f, 1.0f },
                { -0.95f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f }
            };

            D3D11_BUFFER_DESC desc{};
            desc.Usage = D3D11_USAGE_DYNAMIC;
            desc.ByteWidth = sizeof(vertices);
            desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
            desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
            m_buffers[0].buffer = m_renderer.CreateBuffer(desc, vertices);
            m_buffers[0].stride = sizeof(Vertex);
            m_buffers[0].topo = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
            m_buffers[0].cpuData.resize(desc.ByteWidth);
            memcpy(m_buffers[0].cpuData.data(), vertices, desc.ByteWidth);
        }

        {// Create vertex buffer
            Vertex vertices[] = {
                {  0.50f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f },
                {  0.95f, -0.5,  0.0f, 0.0f, 1.0f, 0.0f, 1.0f },
                {  0.05f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f }
            };

            D3D11_BUFFER_DESC desc{};
            desc.Usage = D3D11_USAGE_DYNAMIC;
            desc.ByteWidth = sizeof(vertices);
            desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
            desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
            m_buffers[1].buffer = m_renderer.CreateBuffer(desc, vertices);
            m_buffers[1].stride = sizeof(Vertex);
            m_buffers[1].topo = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
            m_buffers[1].cpuData.resize(desc.ByteWidth);
            memcpy(m_buffers[1].cpuData.data(), vertices, desc.ByteWidth);
        }

        {
            VertexUi vertices[] = {
                { -1.0f, -1.0f, 0.0f, 1.0f },
                { -1.0f,  1.0f, 0.0f, 0.0f },
                {  1.0f, -1.0f, 1.0f, 1.0f },
                {  1.0f,  1.0f, 1.0f, 0.0f },
            };

            D3D11_BUFFER_DESC desc{};
            desc.Usage = D3D11_USAGE_DYNAMIC;
            desc.ByteWidth = sizeof(vertices);
            desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
            desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
            m_buffers[2].buffer = m_renderer.CreateBuffer(desc, vertices);
            m_buffers[2].stride = sizeof(VertexUi);
            m_buffers[2].topo = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
            m_buffers[2].cpuData.resize(desc.ByteWidth);
            memcpy(m_buffers[2].cpuData.data(), vertices, desc.ByteWidth);
        }

        // Create textures
        for (auto& rt : m_renderTargets) {
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
            device->CreateSamplerState(&samplerDesc, m_sampler.Receive());
        }
    };
}

bool GraphicsSystem::Update(IApplication* iapp) {
    UnusedVar(iapp);
    auto device = m_renderer.GetDevice();
    float clearColor[4] {
        0.0f, 0.2f, 0.4f, 1.0f
    };

    static float angle = 0.f;
    constexpr float delta_angle = 0.001f;
    angle += delta_angle;

    {// This code moves vertices
        {
            BufferMapper<Vertex> mapper(m_buffers[0].buffer.Get(), device->GetContext(), D3D11_MAP_WRITE_DISCARD);
            auto view = m_buffers[0].MakeViewCPU<Vertex>();
            for (size_t i = 0; i < view.GetSize(); ++i) {
                view[i].pos.x += 0.003f * std::sin(angle);
                mapper.At(i) = view[i];
            }
        }

        {
            BufferMapper<Vertex> mapper(m_buffers[1].buffer.Get(), device->GetContext(), D3D11_MAP_WRITE_DISCARD);
            auto view = m_buffers[1].MakeViewCPU<Vertex>();
            for (size_t i = 0; i < view.GetSize(); ++i) {
                view[i].pos.x += 0.003f * std::sin(angle + 3.14159f);
                mapper.At(i) = view[i];
            }
        }
    }

    for(int i = 0; i < 2; ++i) {
        device->GetContext()->ClearRenderTargetView(m_renderTargets[i].rt_rtv->GetView(), clearColor);
        device->GetContext()->ClearDepthStencilView(m_renderTargets[i].ds_dsv->GetView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
        m_renderTargets[i].Activate(device.Get());
        m_drawShader.Activate(device.Get());
        m_buffers[i].Activate(device.Get());
        m_renderer.Draw(3, 0);
    }

    {
        device->GetContext()->ClearRenderTargetView(m_renderTargets[2].rt_rtv->GetView(), clearColor);
        device->GetContext()->ClearDepthStencilView(m_renderTargets[2].ds_dsv->GetView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
        m_renderTargets[2].Activate(device.Get());

        ID3D11ShaderResourceView* textures[] {
            m_renderTargets[0].rt_srv->GetView(),
            m_renderTargets[1].rt_srv->GetView(),
            m_renderTargets[0].ds_srv->GetView(),
            m_renderTargets[1].ds_srv->GetView(),
        };
        device->GetContext()->PSSetShaderResources(0, 4, textures);

        auto sampler = m_sampler.Get();
        device->GetContext()->PSSetSamplers(0, 1, &sampler);
        m_uiShader.Activate(device.Get());
        m_buffers[2].Activate(device.Get());
        m_renderer.Draw(4, 0);

        // Unset texture
        ZeroMemory(textures, sizeof(textures));
        device->GetContext()->PSSetShaderResources(0, 4, textures);

        // Unset sampler
        sampler = nullptr;
        device->GetContext()->PSSetSamplers(0, 1, &sampler);
    }

    {// Select render target to present
        int iRT = 0;
        if (HIWORD(GetKeyState(VK_F2))) {
            iRT = 1;
        }
        else if (HIWORD(GetKeyState(VK_F3))) {
            iRT = 2;
        }

        ID3D11Resource* finalRT;
        auto finalRenderTarget = m_renderer.GetSwapchainRenderTargetView();
        finalRenderTarget->GetView()->GetResource(&finalRT);
        auto renderTargetToShow = static_cast<ID3D11Texture2D*>(m_renderTargets[iRT].rt->GetNativeInterface());
        device->GetContext()->CopyResource(finalRT, renderTargetToShow);
    }

    m_renderer.Present();

    return true;
}

void GraphicsSystem::OnWindowResize(int w, int h) {
    UnusedVar(w, h);
    OutputDebugStringA("Resize\n");
}