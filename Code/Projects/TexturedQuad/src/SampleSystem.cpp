#include "SampleSystem.h"
#include "EverydayTools/Array/ArrayViewVector.h"
#include "EverydayTools/Exception/CheckedCast.h"
#include "EverydayTools/Geom/Vector.h"
#include "Keng/Core/IApplication.h"
#include "Keng/GraphicsCommon/DepthStencilParameters.h"
#include "Keng/GraphicsCommon/PrimitiveTopology.h"
#include "Keng/GraphicsCommon/SamplerParameters.h"
#include "Keng/GraphicsCommon/TextureRenderTargetParameters.h"
#include "Keng/GraphicsCommon/ViewportParameters.h"
#include "Keng/GraphicsCommon/WindowRenderTargetParameters.h"
#include "Keng/Graphics/IDevice.h"
#include "Keng/Graphics/IGraphicsSystem.h"
#include "Keng/Graphics/Resource/ITexture.h"
#include "Keng/Graphics/Resource/IEffect.h"
#include "Keng/GPU/PipelineInput/ISampler.h"
#include "Keng/GPU/DeviceBufferMapper.h"
#include "Keng/GPU/RenderTarget/IWindowRenderTarget.h"
#include "Keng/GPU/RenderTarget/ITextureRenderTarget.h"
#include "Keng/GPU/RenderTarget/IDepthStencil.h"
#include "Keng/GPU/Resource/TextureParameters.h"
#include "Keng/GPU/ScopedAnnotation.h"
#include "Keng/ResourceSystem/IResourceSystem.h"
#include "Keng/WindowSystem/IWindowSystem.h"
#include "Keng/WindowSystem/IWindow.h"

namespace textured_quad_sample
{
    namespace
    {
        struct Vertex
        {
            edt::geom::Vector<float, 4> pos;
            edt::geom::Vector<float, 2> tex;
        };

        struct CB
        {
            edt::geom::Matrix<float, 4, 4> transform;
        };

        template<typename T>
        edt::geom::Matrix<T, 4, 4> MakeTranslationMatrix(edt::geom::Vector<T, 3> vec) {
            auto m = edt::geom::Matrix<T, 4, 4>::Identity();

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
                constexpr float delta_angle = 0.002f;
                angle += delta_angle;

                Annotate(m_annotation, L"Move triangle", [&] {
                    gpu::DeviceBufferMapper mapper;
                    m_constantBuffer->MakeMapper(mapper);
                    auto cbView = mapper.GetTypedView<CB>();
                    edt::geom::Vector<float, 3> t{};
                    t.rx() = std::sin(angle);
                    t.ry() = std::sin(angle);
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
                    m_depthStencil->Clear(gpu::DepthStencilClearFlags::ClearDepth | gpu::DepthStencilClearFlags::ClearStencil, 1.0f, 0);
                    m_textureRT->AssignToPipeline(m_depthStencil);
                    m_effect->AssignToPipeline();
                    m_vertexBuffer->AssignToPipeline(vbAssignParams);
                    m_graphicsSystem->GetDevice()->GetApiDevice()->SetTopology(PrimitiveTopology::TriangleStrip);
                    m_constantBuffer->AssignToPipeline(cbAssignParams);
                    m_sampler->AssignToPipeline(ShaderType::Fragment, 0);
                    m_texture->GetApiTexture()->AssignToPipeline(ShaderType::Fragment, 0);
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
        return "TexturedQuad";
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
            m_resourceSystem = app->FindSystem<IResourceSystem>();
            m_graphicsSystem = app->FindSystem<IGraphicsSystem>();
            m_windowSystem = app->FindSystem<IWindowSystem>();
            auto api_device = m_graphicsSystem->GetDevice()->GetApiDevice();
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
                gpu::TextureParameters rtTextureParams{};
                rtTextureParams.format = FragmentFormat::R8_G8_B8_A8_UNORM;
                rtTextureParams.width = w;
                rtTextureParams.height = h;
                rtTextureParams.usage = TextureUsage::ShaderResource | TextureUsage::RenderTarget;
                auto renderTargetTexture = m_graphicsSystem->CreateTexture(rtTextureParams)->GetApiTexture();
                m_textureRT = api_device->CreateTextureRenderTarget(texture_rt_params, *renderTargetTexture);
            }

            {// Create depth stencil
                DepthStencilParameters depthStencilParams{};
                gpu::TextureParameters dsTextureParams{};
                dsTextureParams.format = FragmentFormat::R24_G8_TYPELESS;
                dsTextureParams.width = w;
                dsTextureParams.height = h;
                dsTextureParams.usage = TextureUsage::ShaderResource | TextureUsage::DepthStencil;

                depthStencilParams.format = FragmentFormat::D24_UNORM_S8_UINT;
                auto depthStencilTexture = m_graphicsSystem->CreateTexture(dsTextureParams)->GetApiTexture();
                m_depthStencil = api_device->CreateDepthStencil(depthStencilParams, *depthStencilTexture);
            }

            m_texture = std::static_pointer_cast<ITexture>(m_resourceSystem->GetResource("Assets/Textures/container.json", m_graphicsSystem->GetDevice()));

            {// Read and compile shaders
                std::string_view effectName = "Assets/Effects/Textured.json";
                m_effect = std::static_pointer_cast<IEffect>(m_resourceSystem->GetResource(effectName.data(), m_graphicsSystem->GetDevice()));
                m_effect->InitDefaultInputLayout();
            }

            {// Create vertex buffer
                Vertex vertices[4];

                //      POSITION               ////         TEXTURE COORDS       /**/
                /////////////////////////////////////////////////////////////////////
                vertices[0].pos.rx() = -0.50f; /**/ vertices[0].tex.rx() = 0.0f; /**/
                vertices[0].pos.ry() = -0.50f; /**/ vertices[0].tex.ry() = 0.0f; /**/
                vertices[0].pos.rz() = +0.00f; /**/                              /**/
                vertices[0].pos.rw() = +1.00f; /**/                              /**/
                /////////////////////////////////////////////////////////////////////
                vertices[1].pos.rx() = -0.50f; /**/ vertices[1].tex.rx() = 0.0f; /**/
                vertices[1].pos.ry() = +0.50f; /**/ vertices[1].tex.ry() = 1.0f; /**/
                vertices[1].pos.rz() = +0.00f; /**/                              /**/
                vertices[1].pos.rw() = +1.00f; /**/                              /**/
                /////////////////////////////////////////////////////////////////////
                vertices[2].pos.rx() = +0.50f; /**/ vertices[2].tex.rx() = 1.0f; /**/
                vertices[2].pos.ry() = -0.50f; /**/ vertices[2].tex.ry() = 0.0f; /**/
                vertices[2].pos.rz() = +0.00f; /**/                              /**/
                vertices[2].pos.rw() = +1.00f; /**/                              /**/
                /////////////////////////////////////////////////////////////////////
                vertices[3].pos.rx() = +0.50f; /**/ vertices[3].tex.rx() = 1.0f; /**/
                vertices[3].pos.ry() = +0.50f; /**/ vertices[3].tex.ry() = 1.0f; /**/
                vertices[3].pos.rz() = +0.00f; /**/                              /**/
                vertices[3].pos.rw() = +1.00f; /**/                              /**/
                /////////////////////////////////////////////////////////////////////

                gpu::DeviceBufferParameters params;
                params.size = sizeof(vertices);
                params.usage = gpu::DeviceBufferUsage::Dynamic;
                params.bindFlags = gpu::DeviceBufferBindFlags::VertexBuffer;
                params.accessFlags = gpu::DeviceAccessFlags::Write;
                m_vertexBuffer = m_graphicsSystem->GetDevice()->GetApiDevice()->CreateDeviceBuffer(params, edt::DenseArrayView<uint8_t>((uint8_t*)&vertices, sizeof(vertices)));
            }

            {// Create constant buffer
                CB constantBufferInitData;
                edt::geom::Vector<float, 3> t {};
                constantBufferInitData.transform = MakeTranslationMatrix(t);

                gpu::DeviceBufferParameters params;
                params.size = sizeof(constantBufferInitData);
                params.usage = gpu::DeviceBufferUsage::Dynamic;
                params.bindFlags = gpu::DeviceBufferBindFlags::ConstantBuffer;
                params.accessFlags = gpu::DeviceAccessFlags::Write;
                m_constantBuffer = m_graphicsSystem->GetDevice()->GetApiDevice()->CreateDeviceBuffer(params, edt::DenseArrayView<uint8_t>((uint8_t*)&constantBufferInitData, sizeof(constantBufferInitData)));
            }

            {// Create sampler
                SamplerParameters samplerParams{};
                samplerParams.addressU = TextureAddressMode::Clamp;
                samplerParams.addressV = TextureAddressMode::Clamp;
                samplerParams.addressW = TextureAddressMode::Clamp;
                samplerParams.filter = FilteringMode::Anisotropic;
                m_sampler = api_device->CreateSampler(samplerParams);
            }
        };
    }

}
