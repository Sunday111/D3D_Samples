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
}

GraphicsSystem::GraphicsSystem(Application* app, Renderer::CreateParams& params) :
    m_renderer(params),
    m_app(app)
{
    app->GetWindowSystem()->GetWindow()->Subscribe(this);

    {// Read and compile shaders
        m_vertexShader = CreateShaderFromFile<ShaderType::Vertex>("Assets/Shaders/Shader.hlsl", "VShader", ShaderVersion::_5_0);
        m_pixelShader = CreateShaderFromFile<ShaderType::Pixel>("Assets/Shaders/Shader.hlsl", "PShader", ShaderVersion::_5_0);
    }

    {// Create vertex buffer
        Vertex vertices[] = {
            { 0.0f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f },
            { 0.45f, -0.5, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f },
            { -0.45f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f }
        };

        D3D11_BUFFER_DESC desc{};
        desc.Usage = D3D11_USAGE_DYNAMIC;
        desc.ByteWidth = sizeof(vertices);
        desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

        m_vertices = m_renderer.CreateBuffer(desc, vertices);
    }

    {
        uint32_t w = 1280, h = 720;
        auto renderTarget = m_renderer.CreateTexture(w, h, TextureFormat::R8_G8_B8_A8_UNORM, TextureFlags::RenderTarget | TextureFlags::ShaderResource);
        auto depthStencil = m_renderer.CreateTexture(w, h, TextureFormat::R24_G8_TYPELESS, TextureFlags::DepthStencil | TextureFlags::ShaderResource);
        auto rtv = m_renderer.CreateTextureView<ResourceViewType::RenderTarget>(renderTarget.get(), renderTarget->GetFormat());
        auto dsv = m_renderer.CreateTextureView<ResourceViewType::DepthStencil>(depthStencil.get(), TextureFormat::D24_UNORM_S8_UINT);
    }

    {
        D3D11_INPUT_ELEMENT_DESC elementDesc[]{
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
        };
        m_inputLayout = m_renderer.CreateInputLayout(elementDesc, 2, m_vertexShader.bytecode.Get());
    }
}

bool GraphicsSystem::Update(IApplication* iapp) {
    {// Change vertex buffer to see runtime result
        BufferMapper<Vertex> bufferMap(m_vertices.Get(), m_renderer.GetDevice()->GetContext(), D3D11_MAP_WRITE_DISCARD);
        Vertex vertices[] = {
            {  0.0f,   0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f },
            {  0.45f, -0.5,  0.0f, 0.0f, 1.0f, 0.0f, 1.0f },
            { -0.45f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f }
        };

        static float angle = 0.f;
        constexpr float da = 0.01f;

        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                vertices[i].col.arr[j] = angle - std::floor(angle);
            }
        }

        bufferMap.Write(vertices, 3);
        angle += da;
    }
    UnusedVar(iapp);
    auto device = m_renderer.GetDevice();
    device->SetShader(m_vertexShader);
    device->SetShader(m_pixelShader);
    m_renderer.SetInputLayout(m_inputLayout.Get());
    m_renderer.SetVertexBuffer(m_vertices.Get(), sizeof(Vertex), 0);
    m_renderer.SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    m_renderer.Clear(0.0f, 0.2f, 0.4f, 1.0f);
    {
        D3D11_VIEWPORT vp{ 0, 0, 500, 500, 1.f, 1.f };
        m_renderer.SetViewport(vp);
        m_renderer.Draw(3, 0);
    }

    {
        D3D11_VIEWPORT vp{ 500, 0, 500, 500, 1.f, 1.f };
        m_renderer.SetViewport(vp);
        m_renderer.Draw(3, 0);
    }


    m_renderer.Present();

    return true;
}

void GraphicsSystem::OnWindowResize(int w, int h) {
    UnusedVar(w, h);
    OutputDebugStringA("Resize\n");
}