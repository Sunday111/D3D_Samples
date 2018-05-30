#include "SampleSystem.h"
#include "EverydayTools/Array/ArrayViewVector.h"
#include "EverydayTools/Geom/Vector.h"
#include "EverydayTools/Exception/CheckedCast.h"
#include "Keng/Core/IApplication.h"
#include "Keng/Graphics/Resource/ITexture.h"
#include "Keng/Graphics/Resource/IEffect.h"
#include "Keng/GraphicsAPI/Resource/TextureParameters.h"
#include "Keng/GraphicsAPI/RenderTarget/IWindowRenderTarget.h"
#include "Keng/GraphicsAPI/RenderTarget/WindowRenderTargetParameters.h"
#include "Keng/GraphicsAPI/RenderTarget/ITextureRenderTarget.h"
#include "Keng/GraphicsAPI/RenderTarget/TextureRenderTargetParameters.h"
#include "Keng/GraphicsAPI/RenderTarget/IDepthStencil.h"
#include "Keng/GraphicsAPI/RenderTarget/DepthStencilParameters.h"
#include "Keng/GraphicsAPI/DeviceBufferMapper.h"
#include "Keng/GraphicsAPI/ViewportParameters.h"
#include "Keng/GraphicsAPI/ScopedAnnotation.h"
#include "Keng/ResourceSystem/IResourceSystem.h"
#include "Keng/WindowSystem/IWindowSystem.h"
#include "Keng/WindowSystem/IWindow.h"
#include "Keng/Graphics/IGraphicsSystem.h"
#include "Keng/Graphics/IDevice.h"

namespace simple_quad_sample
{
    namespace
    {
        struct Vertex
        {
            edt::geom::Vector<float, 4> pos;
            edt::geom::Vector<float, 4> col;
        };

        struct CB
        {
            edt::geom::Matrix<float, 4, 4> transform;
        };

        template<typename T>
        edt::geom::Matrix<T, 4, 4> MakeIdentityMatrix() {
            edt::geom::Matrix<T, 4, 4> m{};
            m.At(0, 0) = T(1);
            m.At(1, 1) = T(1);
            m.At(2, 2) = T(1);
            m.At(3, 3) = T(1);
            return m;
        }

        template<typename T>
        edt::geom::Matrix<T, 4, 4> MakeTranslationMatrix(edt::geom::Vector<T, 3> vec) {
            auto m = MakeIdentityMatrix<T>();

            m.At(3, 0) = vec.Elem(0);
            m.At(3, 1) = vec.Elem(1);
            m.At(3, 2) = vec.Elem(2);

            return m;
        }
    }

    SampleSystem::SampleSystem() = default;

    SampleSystem::~SampleSystem() = default;

    bool SampleSystem::Update() {
        using namespace keng;
        using namespace graphics;

        return CallAndRethrowM + [&] {
            return Annotate(m_annotation, L"Frame", [&] {
                float clearColor[4]{
                    0.0f, 0.2f, 0.4f, 1.0f
                };

                static float angle = 0.f;
                constexpr float delta_angle = 0.001f;
                angle += delta_angle;

                Annotate(m_annotation, L"Move triangle", [&] {
                    graphics_api::DeviceBufferMapper mapper;
                    m_constantBuffer->MakeMapper(mapper);
                    auto cbView = mapper.GetTypedView<CB>();
                    edt::geom::Vector<float, 3> t;
                    t.rx() = std::sin(angle);
                    t.ry() = 0.f;
                    t.rz() = 0.f;
                    cbView[0].transform = MakeTranslationMatrix(t);
                });

                Annotate(m_annotation, L"Draw triangle", [&] {
                    graphics_api::VertexBufferAssignParameters vbAssignParams{};
                    vbAssignParams.slot = 0;
                    vbAssignParams.stride = sizeof(Vertex);

                    graphics_api::ConstantBufferAssignParameters cbAssignParams{};
                    cbAssignParams.slot = 0;
                    cbAssignParams.stride = sizeof(CB);
                    cbAssignParams.shaderType = graphics_api::ShaderType::Vertex;

                    m_textureRT->Clear(clearColor);
                    m_depthStencil->Clear(graphics_api::DepthStencilClearFlags::ClearDepth | graphics_api::DepthStencilClearFlags::ClearStencil, 1.0f, 0);
                    m_textureRT->AssignToPipeline(m_depthStencil);
                    m_effect->AssignToPipeline();
                    m_vertexBuffer->AssignToPipeline(vbAssignParams);
                    m_graphicsSystem->SetTopology(graphics_api::PrimitiveTopology::TriangleStrip);
                    m_constantBuffer->AssignToPipeline(cbAssignParams);
                    m_graphicsSystem->Draw(4, 0);
                });

                Annotate(m_annotation, L"Copy texture to swap chain texture", [&] {
                    m_windowRT->CopyFrom(m_textureRT->GetTexture());
                });

                m_windowRT->Present();

                return true;
            });
        };
    }

    void SampleSystem::Shutdown() {

    }

    const char* SampleSystem::GetSystemName() const {
        return "SimpleQuad";
    }

    bool SampleSystem::ForEachDependency(const edt::Delegate<bool(const char*)>& delegate) const {
        if (delegate.Invoke(keng::graphics::IGraphicsSystem::SystemName())) return true;
        if (delegate.Invoke(keng::resource::IResourceSystem::SystemName())) return true;
        if (delegate.Invoke(keng::window_system::IWindowSystem::SystemName())) return true;
        return false;
    }

    void SampleSystem::Initialize(const keng::core::IApplicationPtr& app) {
        using namespace keng;
        using namespace graphics;
        using namespace resource;
        using namespace window_system;

        CallAndRethrowM + [&] {
            m_resourceSystem = app->FindSystem<resource::IResourceSystem>();
            m_graphicsSystem = app->FindSystem<graphics::IGraphicsSystem>();
            m_windowSystem = app->FindSystem<window_system::IWindowSystem>();
            m_annotation = m_graphicsSystem->CreateAnnotation();

            auto window = m_windowSystem->GetWindow();
            size_t w, h;
            window->GetClientSize(&w, &h);

            {// Initialize viewport
                graphics_api::ViewportParameters v{};
                v.Position.rx() = 0.f;
                v.Position.ry() = 0.f;
                v.Size.rx() = edt::CheckedCast<float>(w);
                v.Size.ry() = edt::CheckedCast<float>(h);
                m_graphicsSystem->SetViewport(v);
            }

            {// Create window render target
                graphics_api::WindowRenderTargetParameters window_rt_params;
                window_rt_params.swapChain.format = graphics_api::FragmentFormat::R8_G8_B8_A8_UNORM;
                window_rt_params.swapChain.window = window;
                window_rt_params.swapChain.buffers = 2;
                m_windowRT = m_graphicsSystem->CreateWindowRenderTarget(window_rt_params);
            }

            {// Create texture render target
                graphics_api::TextureRenderTargetParameters texture_rt_params{};
                graphics_api::TextureParameters rtTextureParams{};
                rtTextureParams.format = graphics_api::FragmentFormat::R8_G8_B8_A8_UNORM;
                rtTextureParams.width = w;
                rtTextureParams.height = h;
                rtTextureParams.usage = graphics_api::TextureUsage::ShaderResource | graphics_api::TextureUsage::RenderTarget;
                texture_rt_params.renderTarget = m_graphicsSystem->CreateTexture(rtTextureParams)->GetApiTexture();
                m_textureRT = m_graphicsSystem->CreateTextureRenderTarget(texture_rt_params);
            }

            {// Create depth stencil
                graphics_api::DepthStencilParameters depthStencilParams{};
                graphics_api::TextureParameters dsTextureParams{};
                dsTextureParams.format = graphics_api::FragmentFormat::R24_G8_TYPELESS;
                dsTextureParams.width = w;
                dsTextureParams.height = h;
                dsTextureParams.usage = graphics_api::TextureUsage::ShaderResource | graphics_api::TextureUsage::DepthStencil;

                depthStencilParams.format = graphics_api::FragmentFormat::D24_UNORM_S8_UINT;
                depthStencilParams.texture = m_graphicsSystem->CreateTexture(dsTextureParams)->GetApiTexture();
                m_depthStencil = m_graphicsSystem->CreateDepthStencil(depthStencilParams);
            }

            {// Read and compile shaders
                std::string_view effectName = "Assets/Effects/FlatColor.json";
                m_effect = std::static_pointer_cast<IEffect>(m_resourceSystem->GetResource(effectName.data(), m_graphicsSystem->GetDevice()));
                m_effect->InitDefaultInputLayout();
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

                graphics_api::DeviceBufferParameters params;
                params.size = sizeof(vertices);
                params.usage = graphics_api::DeviceBufferUsage::Dynamic;
                params.bindFlags = graphics_api::DeviceBufferBindFlags::VertexBuffer;
                params.accessFlags = graphics_api::DeviceAccessFlags::Write;
                m_vertexBuffer = m_graphicsSystem->CreateDeviceBuffer(params, edt::DenseArrayView<uint8_t>((uint8_t*)&vertices, sizeof(vertices)));
            }

            {// Create constant buffer
                CB constantBufferInitData;
                edt::geom::Vector<float, 3> t {};
                constantBufferInitData.transform = MakeTranslationMatrix(t);

                graphics_api::DeviceBufferParameters params;
                params.size = sizeof(constantBufferInitData);
                params.usage = graphics_api::DeviceBufferUsage::Dynamic;
                params.bindFlags = graphics_api::DeviceBufferBindFlags::ConstantBuffer;
                params.accessFlags = graphics_api::DeviceAccessFlags::Write;
                m_constantBuffer = m_graphicsSystem->CreateDeviceBuffer(params, edt::DenseArrayView<uint8_t>((uint8_t*)&constantBufferInitData, sizeof(constantBufferInitData)));
            }
        };
    }
}
