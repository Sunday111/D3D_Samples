#include "System.h"
#include "EverydayTools/Delegate.h"
#include "EverydayTools/Array/ArrayViewVector.h"
#include "EverydayTools/Geom/Vector.h"
#include "Keng/Core/IApplication.h"
#include "Keng/Graphics/Resource/Font/IFont.h"
#include "Keng/Graphics/Resource/Font/GlyphInfo.h"
#include "Keng/Graphics/Resource/Font/GlyphParameters.h"
#include "Keng/Graphics/Resource/ITexture.h"
#include "Keng/Graphics/Resource/IEffect.h"
#include "Keng/Graphics/Resource/TextureParameters.h"
#include "Keng/Graphics/RenderTarget/IWindowRenderTarget.h"
#include "Keng/Graphics/RenderTarget/WindowRenderTargetParameters.h"
#include "Keng/Graphics/RenderTarget/ITextureRenderTarget.h"
#include "Keng/Graphics/RenderTarget/TextureRenderTargetParameters.h"
#include "Keng/Graphics/RenderTarget/IDepthStencil.h"
#include "Keng/Graphics/RenderTarget/DepthStencilParameters.h"
#include "Keng/Graphics/DeviceBufferMapper.h"
#include "Keng/Graphics/ViewportParameters.h"
#include "Keng/Graphics/SamplerParameters.h"
#include "Keng/Graphics/ISampler.h"
#include "Keng/Graphics/ScopedAnnotation.h"
#include "Keng/ResourceSystem/IResourceSystem.h"
#include "Keng/WindowSystem/IWindowSystem.h"
#include "Keng/WindowSystem/IWindow.h"
#include "Keng/Graphics/IGraphicsSystem.h"
#include "Keng/Graphics/IDevice.h"

#include "stdlib.h"


namespace render_text_sample
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

    System::System() = default;

    System::~System() = default;

    bool System::Update() {
        using namespace keng;
        using namespace graphics;

        return CallAndRethrowM + [&] {
            return Annotate(m_annotation, "Frame", [&] {
                float clearColor[4]{
                    0.0f, 0.2f, 0.4f, 1.0f
                };

                static float angle = 0.f;
                constexpr float delta_angle = 0.000f;
                angle += delta_angle;

                Annotate(m_annotation, "Edit constant buffer", [&] {
                    DeviceBufferMapper mapper;
                    m_constantBuffer->MakeMapper(mapper);
                    auto cbView = mapper.GetTypedView<CB>();
                    edt::geom::Vector<float, 3> t{};
                    t.rx() = std::sin(angle);
                    t.ry() = std::sin(angle);
                    cbView[0].transform = MakeTranslationMatrix(t);
                });

                Annotate(m_annotation, "Draw", [&] {
                    Annotate(m_annotation, "Clear", [&] {
                        m_windowRT->Clear(clearColor);
                        m_depthStencil->Clear(DepthStencilClearFlags::ClearDepth | DepthStencilClearFlags::ClearStencil, 1.0f, 0);
                        m_windowRT->AssignToPipeline(m_depthStencil);
                    });

                    Annotate(m_annotation, "Assign buffers", [&] {
                        VertexBufferAssignParameters vbAssignParams {};
                        vbAssignParams.slot = 0;
                        vbAssignParams.stride = sizeof(Vertex);
                        m_vertexBuffer->AssignToPipeline(vbAssignParams);

                        ConstantBufferAssignParameters cbAssignParams {};
                        cbAssignParams.slot = 0;
                        cbAssignParams.stride = sizeof(CB);
                        cbAssignParams.shaderType = ShaderType::Vertex;
                        m_constantBuffer->AssignToPipeline(cbAssignParams);
                    });

                    Annotate(m_annotation, "Setup effect", [&] {
                        m_effect->AssignToPipeline();
                        m_graphicsSystem->SetTopology(PrimitiveTopology::TriangleStrip);
                        m_sampler->AssignToPipeline(ShaderType::Fragment, 0);
                        m_texture->AssignToPipeline(ShaderType::Fragment, 0);
                    });

                    m_graphicsSystem->Draw(4, 0);
                });

                m_windowRT->Present();

                return true;
            });
        };
    }

    std::string_view System::GetSystemName() const {
        return "RenderText";
    }

    bool System::ForEachSystemDependency(bool(*pfn)(std::string_view systemName, void* context), void* context) const {
        if (pfn(keng::graphics::IGraphicsSystem::SystemName(), context)) return true;
        if (pfn(keng::resource::IResourceSystem::SystemName(), context)) return true;
        if (pfn(keng::window_system::IWindowSystem::SystemName(), context)) return true;
        return false;
    }

    void System::Initialize(const keng::core::IApplicationPtr& app) {
        using namespace keng;
        using namespace graphics;
        using namespace resource;
        using namespace window_system;

        CallAndRethrowM + [&] {
            m_resourceSystem = app->FindSystem<IResourceSystem>();
            m_graphicsSystem = app->FindSystem<IGraphicsSystem>();
            m_windowSystem = app->FindSystem<IWindowSystem>();
            m_annotation = m_graphicsSystem->CreateAnnotation();

            auto window = m_windowSystem->GetWindow();
            uint32_t w, h;
            window->GetClientSize(&w, &h);

            {// Initialize viewport
                ViewportParameters v{};
                v.Position.rx() = 0.f;
                v.Position.ry() = 0.f;
                v.Size.rx() = static_cast<float>(w);
                v.Size.ry() = static_cast<float>(h);
                m_graphicsSystem->SetViewport(v);
            }

            {// Create window render target
                WindowRenderTargetParameters window_rt_params;
                window_rt_params.swapChain.format = FragmentFormat::R8_G8_B8_A8_UNORM;
                window_rt_params.swapChain.window = window;
                window_rt_params.swapChain.buffers = 2;
                m_windowRT = m_graphicsSystem->CreateWindowRenderTarget(window_rt_params);
            }

            {// Create depth stencil
                DepthStencilParameters depthStencilParams{};
                TextureParameters dsTextureParams{};
                dsTextureParams.format = FragmentFormat::R24_G8_TYPELESS;
                dsTextureParams.width = w;
                dsTextureParams.height = h;
                dsTextureParams.usage = TextureUsage::ShaderResource | TextureUsage::DepthStencil;

                depthStencilParams.format = FragmentFormat::D24_UNORM_S8_UINT;
                depthStencilParams.texture = m_graphicsSystem->CreateTexture(dsTextureParams);
                m_depthStencil = m_graphicsSystem->CreateDepthStencil(depthStencilParams);
            }

            //m_texture = std::static_pointer_cast<ITexture>(m_resourceSystem->GetResource("Assets/Textures/container.json", m_graphicsSystem->GetDevice()));

            auto font = std::static_pointer_cast<IFont>(m_resourceSystem->GetResource("Assets/Fonts/OpenSans.json"));
            GlyphParameters glyphParams{};
            glyphParams.size = 16;
            glyphParams.dpiX = 96;
            glyphParams.dpiY = 96;

            std::vector<uint32_t> unicodes;
            for (char letter : "ABCDEFGHIJKLMNOPQRSTUVWXYZ abcdefghijklmnopqrstuvwxyz 1234567890 !?.,;:\\|/*-+") {
                unicodes.push_back(letter);
            }

            auto onRequest = [&](const AtlasGlyphInfo& info) {
                m_texture = info.texture;
            };

            edt::Delegate<void(const AtlasGlyphInfo&)> delegate;
            delegate.Bind(onRequest);
            font->RequestGlyphsInfo(edt::MakeArrayView(unicodes), *m_graphicsSystem->GetDevice(), glyphParams, delegate);

            {// Read and compile shaders
                std::string_view effectName = "Assets/Effects/GrayscaleTexture.json";
                m_effect = std::static_pointer_cast<IEffect>(m_resourceSystem->GetResource(effectName, m_graphicsSystem->GetDevice()));
                m_effect->InitDefaultInputLayout();
            }

            {// Create vertex buffer
                Vertex vertices[4];

                //      POSITION               ////         TEXTURE COORDS       /**/
                /////////////////////////////////////////////////////////////////////
                vertices[0].pos.rx() = -0.50f; /**/ vertices[0].tex.rx() = 0.0f; /**/
                vertices[0].pos.ry() = -0.50f; /**/ vertices[0].tex.ry() = 1.0f; /**/
                vertices[0].pos.rz() = +0.00f; /**/                              /**/
                vertices[0].pos.rw() = +1.00f; /**/                              /**/
                /////////////////////////////////////////////////////////////////////
                vertices[1].pos.rx() = -0.50f; /**/ vertices[1].tex.rx() = 0.0f; /**/
                vertices[1].pos.ry() = +0.50f; /**/ vertices[1].tex.ry() = 0.0f; /**/
                vertices[1].pos.rz() = +0.00f; /**/                              /**/
                vertices[1].pos.rw() = +1.00f; /**/                              /**/
                /////////////////////////////////////////////////////////////////////
                vertices[2].pos.rx() = +0.50f; /**/ vertices[2].tex.rx() = 1.0f; /**/
                vertices[2].pos.ry() = -0.50f; /**/ vertices[2].tex.ry() = 1.0f; /**/
                vertices[2].pos.rz() = +0.00f; /**/                              /**/
                vertices[2].pos.rw() = +1.00f; /**/                              /**/
                /////////////////////////////////////////////////////////////////////
                vertices[3].pos.rx() = +0.50f; /**/ vertices[3].tex.rx() = 1.0f; /**/
                vertices[3].pos.ry() = +0.50f; /**/ vertices[3].tex.ry() = 0.0f; /**/
                vertices[3].pos.rz() = +0.00f; /**/                              /**/
                vertices[3].pos.rw() = +1.00f; /**/                              /**/
                /////////////////////////////////////////////////////////////////////

                graphics::DeviceBufferParams params;
                params.size = sizeof(vertices);
                params.usage = graphics::DeviceBufferUsage::Dynamic;
                params.bindFlags = graphics::DeviceBufferBindFlags::VertexBuffer;
                params.accessFlags = graphics::DeviceAccessFlags::Write;
                m_vertexBuffer = m_graphicsSystem->CreateDeviceBuffer(params, edt::DenseArrayView<uint8_t>((uint8_t*)&vertices, sizeof(vertices)));
            }

            {// Create constant buffer
                CB constantBufferInitData;
                edt::geom::Vector<float, 3> t {};
                constantBufferInitData.transform = MakeTranslationMatrix(t);

                graphics::DeviceBufferParams params;
                params.size = sizeof(constantBufferInitData);
                params.usage = graphics::DeviceBufferUsage::Dynamic;
                params.bindFlags = graphics::DeviceBufferBindFlags::ConstantBuffer;
                params.accessFlags = graphics::DeviceAccessFlags::Write;
                m_constantBuffer = m_graphicsSystem->CreateDeviceBuffer(params, edt::DenseArrayView<uint8_t>((uint8_t*)&constantBufferInitData, sizeof(constantBufferInitData)));
            }

            {// Create sampler
                SamplerParameters samplerParams{};
                samplerParams.addressU = TextureAddressMode::Clamp;
                samplerParams.addressV = TextureAddressMode::Clamp;
                samplerParams.addressW = TextureAddressMode::Clamp;
                samplerParams.filter = FilteringMode::Anisotropic;
                m_sampler = m_graphicsSystem->CreateSampler(samplerParams);
            }
        };
    }

}
