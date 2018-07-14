#include "SampleSystem.h"
#include "EverydayTools/Array/ArrayViewVector.h"
#include "EverydayTools/Geom/Vector.h"
#include "EverydayTools/Exception/CheckedCast.h"
#include "Keng/Core/IApplication.h"
#include "Keng/GPU/DeviceBufferMapper.h"
#include "Keng/GPU/ScopedAnnotation.h"
#include "Keng/GPU/RenderTarget/IWindowRenderTarget.h"
#include "Keng/GPU/RenderTarget/ITextureRenderTarget.h"
#include "Keng/GPU/RenderTarget/IDepthStencil.h"
#include "Keng/GraphicsCommon/DepthStencilClearFlags.h"
#include "Keng/GraphicsCommon/DepthStencilParameters.h"
#include "Keng/GraphicsCommon/DeviceBufferParameters.h"
#include "Keng/GraphicsCommon/PrimitiveTopology.h"
#include "Keng/GraphicsCommon/DeviceTextureParameters.h"
#include "Keng/GraphicsCommon/TextureRenderTargetParameters.h"
#include "Keng/GraphicsCommon/ViewportParameters.h"
#include "Keng/GraphicsCommon/WindowRenderTargetParameters.h"
#include "Keng/Graphics/IDevice.h"
#include "Keng/Graphics/IGraphicsSystem.h"
#include "Keng/Graphics/Resource/ITexture.h"
#include "Keng/Graphics/Resource/IEffect.h"
#include "Keng/ResourceSystem/IResourceSystem.h"
#include "Keng/WindowSystem/IWindowSystem.h"
#include "Keng/WindowSystem/IWindow.h"

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
                auto api_device = m_graphicsSystem->GetDevice()->GetApiDevice();

                float clearColor[4]{
                    0.0f, 0.2f, 0.4f, 1.0f
                };

                static float angle = 0.f;
                constexpr float delta_angle = 0.001f;
                angle += delta_angle;

                Annotate(m_annotation, L"Move triangle", [&] {
                    gpu::DeviceBufferMapper mapper;
                    m_constantBuffer->MakeMapper(mapper);
                    auto cbView = mapper.GetTypedView<CB>();
                    edt::geom::Vector<float, 3> t;
                    t.rx() = std::sin(angle);
                    t.ry() = 0.f;
                    t.rz() = 0.f;
                    cbView[0].transform = MakeTranslationMatrix(t);
                });

                Annotate(m_annotation, L"Draw triangle", [&] {
                    gpu::VertexBufferAssignParameters vbAssignParams{};
                    vbAssignParams.slot = 0;
                    vbAssignParams.stride = sizeof(Vertex);

                    gpu::ConstantBufferAssignParameters cbAssignParams{};
                    cbAssignParams.slot = 0;
                    cbAssignParams.stride = sizeof(CB);
                    cbAssignParams.shaderType = ShaderType::Vertex;

                    m_textureRT->Clear(clearColor);
                    m_depthStencil->Clear(DepthStencilClearFlags::ClearDepth | DepthStencilClearFlags::ClearStencil, 1.0f, 0);
                    m_textureRT->AssignToPipeline(m_depthStencil);
                    m_effect->AssignToPipeline();
                    m_vertexBuffer->AssignToPipeline(vbAssignParams);
                    api_device->SetTopology(PrimitiveTopology::TriangleStrip);
                    m_constantBuffer->AssignToPipeline(cbAssignParams);
                    api_device->Draw(4, 0);
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
            auto api_device = m_graphicsSystem->GetDevice()->GetApiDevice();
            m_windowSystem = app->FindSystem<window_system::IWindowSystem>();
            m_annotation = api_device->CreateAnnotation();

            auto window = m_windowSystem->GetWindow();
            size_t w, h;
            window->GetClientSize(&w, &h);

            {// Initialize viewport
                ViewportParameters v{};
                v.Position.rx() = 0.f;
                v.Position.ry() = 0.f;
                v.Size.rx() = edt::CheckedCast<float>(w);
                v.Size.ry() = edt::CheckedCast<float>(h);
                api_device->SetViewport(v);
            }

            {// Create window render target
                WindowRenderTargetParameters window_rt_params;
                window_rt_params.swapChain.format = FragmentFormat::R8_G8_B8_A8_UNORM;
                window_rt_params.swapChain.buffers = 2;
                m_windowRT = api_device->CreateWindowRenderTarget(window_rt_params, *window);
            }

            {// Create texture render target
                TextureRenderTargetParameters texture_rt_params{};
                DeviceTextureParameters rtTextureParams{};
                rtTextureParams.format = FragmentFormat::R8_G8_B8_A8_UNORM;
                rtTextureParams.width = w;
                rtTextureParams.height = h;
                rtTextureParams.bindFlags = DeviceBufferBindFlags::ShaderResource | DeviceBufferBindFlags::RenderTarget;
                auto renderTragetTexture = m_graphicsSystem->CreateTexture(rtTextureParams)->GetApiTexture();
                m_textureRT = api_device->CreateTextureRenderTarget(texture_rt_params, *renderTragetTexture);
            }

            {// Create depth stencil
                DepthStencilParameters depthStencilParams{};
                DeviceTextureParameters dsTextureParams{};
                dsTextureParams.format = FragmentFormat::R24_G8_TYPELESS;
                dsTextureParams.width = w;
                dsTextureParams.height = h;
                dsTextureParams.bindFlags = DeviceBufferBindFlags::ShaderResource | DeviceBufferBindFlags::DepthStencil;

                depthStencilParams.format = FragmentFormat::D24_UNORM_S8_UINT;
                auto depthStencilTexture = m_graphicsSystem->CreateTexture(dsTextureParams)->GetApiTexture();
                m_depthStencil = api_device->CreateDepthStencil(depthStencilParams, *depthStencilTexture);
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

                DeviceBufferParameters params{};
                params.size = sizeof(vertices);
                params.usage = DeviceBufferUsage::Dynamic;
                params.bindFlags = DeviceBufferBindFlags::VertexBuffer;
                params.accessFlags = CpuAccessFlags::Write;
                m_vertexBuffer = m_graphicsSystem->GetDevice()->GetApiDevice()->CreateDeviceBuffer(params, edt::DenseArrayView<uint8_t>((uint8_t*)&vertices, sizeof(vertices)));
            }

            {// Create constant buffer
                CB constantBufferInitData;
                edt::geom::Vector<float, 3> t {};
                constantBufferInitData.transform = MakeTranslationMatrix(t);

                DeviceBufferParameters params{};
                params.size = sizeof(constantBufferInitData);
                params.usage = DeviceBufferUsage::Dynamic;
                params.bindFlags = DeviceBufferBindFlags::ConstantBuffer;
                params.accessFlags = CpuAccessFlags::Write;
                m_constantBuffer = m_graphicsSystem->GetDevice()->GetApiDevice()->CreateDeviceBuffer(params, edt::DenseArrayView<uint8_t>((uint8_t*)&constantBufferInitData, sizeof(constantBufferInitData)));
            }
        };
    }
}
