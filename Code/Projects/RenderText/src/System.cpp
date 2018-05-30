#include "System.h"
#include "EverydayTools/Delegate.h"
#include "EverydayTools/Array/ArrayViewVector.h"
#include "EverydayTools/Exception/CheckedCast.h"
#include "Keng/Core/IApplication.h"
#include "Keng/Graphics/Resource/Font/IFont.h"
#include "Keng/Graphics/Resource/Font/GlyphInfo.h"
#include "Keng/Graphics/Resource/Font/GlyphParameters.h"
#include "Keng/Graphics/Resource/ITexture.h"
#include "Keng/Graphics/Resource/IEffect.h"
#include "Keng/GPU/Resource/TextureParameters.h"
#include "Keng/GPU/RenderTarget/IWindowRenderTarget.h"
#include "Keng/GPU/RenderTarget/WindowRenderTargetParameters.h"
#include "Keng/GPU/RenderTarget/ITextureRenderTarget.h"
#include "Keng/GPU/RenderTarget/TextureRenderTargetParameters.h"
#include "Keng/GPU/RenderTarget/IDepthStencil.h"
#include "Keng/GPU/RenderTarget/DepthStencilParameters.h"
#include "Keng/GPU/DeviceBufferMapper.h"
#include "Keng/GPU/ViewportParameters.h"
#include "Keng/GPU/SamplerParameters.h"
#include "Keng/GPU/ISampler.h"
#include "Keng/GPU/ScopedAnnotation.h"
#include "Keng/ResourceSystem/IResourceSystem.h"
#include "Keng/WindowSystem/IWindowSystem.h"
#include "Keng/WindowSystem/IWindow.h"
#include "Keng/Graphics/IDevice.h"
#include "Keng/Base/Serialization/SerializeMandatory.h"
#include "Keng/FileSystem/OpenFileParameters.h"
#include "Keng/FileSystem/ReadFileToBuffer.h"
#include "Keng/Base/Serialization/openarchivejson.h"

namespace render_text_sample
{
    namespace
    {

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
            return Annotate(m_annotation, L"Frame", [&] {
                float clearColor[4]{
                    0.0f, 0.2f, 0.4f, 1.0f
                };

                static float angle = 0.f;
                constexpr float delta_angle = 0.0003f;
                angle += delta_angle;


                Annotate(m_annotation, L"Draw", [&] {
                    Annotate(m_annotation, L"Clear", [&] {
                        m_windowRT->Clear(clearColor);
                        m_depthStencil->Clear(gpu::DepthStencilClearFlags::ClearDepth | gpu::DepthStencilClearFlags::ClearStencil, 1.0f, 0);
                        m_windowRT->AssignToPipeline(m_depthStencil);
                    });

                    Annotate(m_annotation, L"Draw quad", [&] {
                        Annotate(m_annotation, L"Edit constant buffer", [&] {
                            gpu::DeviceBufferMapper mapper;
                            m_constantBuffer->MakeMapper(mapper);
                            auto cbView = mapper.GetTypedView<CB>();
                            edt::geom::Vector<float, 3> t{};
                            t.rx() = std::sin(angle);
                            t.ry() = std::sin(angle);
                            cbView[0].transform = MakeTranslationMatrix(t);
                        });

                        Annotate(m_annotation, L"Assign buffers", [&] {
                            m_containerQuad.AssignToPipeline(0, m_graphicsSystem);

                            gpu::ConstantBufferAssignParameters cbAssignParams{};
                            cbAssignParams.slot = 0;
                            cbAssignParams.stride = sizeof(CB);
                            cbAssignParams.shaderType = gpu::ShaderType::Vertex;
                            m_constantBuffer->AssignToPipeline(cbAssignParams);
                        });

                        Annotate(m_annotation, L"Setup effect", [&] {
                            m_texturedEffect->AssignToPipeline();
                            m_containerSampler->AssignToPipeline(gpu::ShaderType::Fragment, 0);
                            m_containerTexture->GetApiTexture()->AssignToPipeline(gpu::ShaderType::Fragment, 0);
                        });

                        m_graphicsSystem->Draw(m_containerQuad.GetVerticesCount(), 0);
                    });

                    m_depthStencil->Clear(gpu::DepthStencilClearFlags::ClearDepth | gpu::DepthStencilClearFlags::ClearStencil, 1.0f, 0);

                    Annotate(m_annotation, L"Draw text", [&] {
                        Annotate(m_annotation, L"Edit constant buffer", [&] {
                            gpu::DeviceBufferMapper mapper;
                            m_constantBuffer->MakeMapper(mapper);
                            auto cbView = mapper.GetTypedView<CB>();
                            cbView[0].transform = MakeIdentityMatrix<float>();
                        });

                        Annotate(m_annotation, L"Assign buffers", [&] {
                            m_textQuads.AssignToPipeline(0, m_graphicsSystem);

                            gpu::ConstantBufferAssignParameters cbAssignParams{};
                            cbAssignParams.slot = 0;
                            cbAssignParams.stride = sizeof(CB);
                            cbAssignParams.shaderType = gpu::ShaderType::Vertex;
                            m_constantBuffer->AssignToPipeline(cbAssignParams);
                        });

                        Annotate(m_annotation, L"Setup effect", [&] {
                            m_textEffect->AssignToPipeline();
                            m_textSampler->AssignToPipeline(gpu::ShaderType::Fragment, 0);
                            m_atlasTexture->GetApiTexture()->AssignToPipeline(gpu::ShaderType::Fragment, 0);
                        });

                        m_graphicsSystem->Draw(m_textQuads.GetVerticesCount(), 0);
                    });
                });

                m_windowRT->Present();

                return true;
            });
        };
    }

    void System::Shutdown() {

    }

    const char* System::GetSystemName() const {
        return "RenderText";
    }

    bool System::ForEachDependency(const edt::Delegate<bool(const char*)>& delegate) const {
        if (delegate.Invoke(keng::graphics::IGraphicsSystem::SystemName())) return true;
        if (delegate.Invoke(keng::resource::IResourceSystem::SystemName())) return true;
        if (delegate.Invoke(keng::window_system::IWindowSystem::SystemName())) return true;
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
            size_t w, h;
            window->GetClientSize(&w, &h);

            {// Initialize viewport
                gpu::ViewportParameters v{};
                v.Position.rx() = 0.f;
                v.Position.ry() = 0.f;
                v.Size.rx() = edt::CheckedCast<float>(w);
                v.Size.ry() = edt::CheckedCast<float>(h);
                m_graphicsSystem->SetViewport(v);
            }

            {// Create window render target
                gpu::WindowRenderTargetParameters window_rt_params;
                window_rt_params.swapChain.format = gpu::FragmentFormat::R8_G8_B8_A8_UNORM;
                window_rt_params.swapChain.window = window;
                window_rt_params.swapChain.buffers = 2;
                m_windowRT = m_graphicsSystem->CreateWindowRenderTarget(window_rt_params);
            }

            {// Create depth stencil
                gpu::DepthStencilParameters depthStencilParams{};
                gpu::TextureParameters dsTextureParams{};
                dsTextureParams.format = gpu::FragmentFormat::R24_G8_TYPELESS;
                dsTextureParams.width = w;
                dsTextureParams.height = h;
                dsTextureParams.usage = gpu::TextureUsage::ShaderResource | gpu::TextureUsage::DepthStencil;

                depthStencilParams.format = gpu::FragmentFormat::D24_UNORM_S8_UINT;
                depthStencilParams.texture = m_graphicsSystem->CreateTexture(dsTextureParams)->GetApiTexture();
                m_depthStencil = m_graphicsSystem->CreateDepthStencil(depthStencilParams);
            }

            m_containerTexture = std::static_pointer_cast<ITexture>(m_resourceSystem->GetResource("Assets/Textures/container.json", m_graphicsSystem->GetDevice()));

            m_font = std::static_pointer_cast<IFont>(m_resourceSystem->GetResource("Assets/Fonts/OpenSans.json"));

            {// Read and compile shaders
                std::string_view effectName = "Assets/Effects/Textured.json";
                m_texturedEffect = std::static_pointer_cast<IEffect>(m_resourceSystem->GetResource(effectName.data(), m_graphicsSystem->GetDevice()));
                m_texturedEffect->InitDefaultInputLayout();
            }

            {// Read and compile shaders
                std::string_view effectName = "Assets/Effects/Text.json";
                m_textEffect = std::static_pointer_cast<IEffect>(m_resourceSystem->GetResource(effectName.data(), m_graphicsSystem->GetDevice()));
                m_textEffect->InitDefaultInputLayout();
            }

            {// Create container vertex buffer
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

                PrimitiveBufferParameters params;
                params.usage = gpu::DeviceBufferUsage::Dynamic;
                params.bindFlags = gpu::DeviceBufferBindFlags::VertexBuffer;
                params.accessFlags = gpu::DeviceAccessFlags::Write;
                params.topology = gpu::PrimitiveTopology::TriangleStrip;
                m_containerQuad.Initialize(*m_graphicsSystem, params, edt::MakeArrayView(vertices));
            }
            
            {// Create text vertex buffer
                GlyphParameters glyphParams{};
                glyphParams.size = 20;
                glyphParams.dpiX = 300;
                glyphParams.dpiY = 300;

                std::vector<uint32_t> unicodes;
                for (char letter : "The quick brown fox jumps over the lazy dog") {
                    unicodes.push_back(letter);
                }

                std::vector<Vertex> vertices;
                vertices.reserve(unicodes.size());
                
                {// Generate triangles per symbol
                    int currentX = - static_cast<int>(w);
                    int currentY = 0;

                    auto onRequest = [&](const AtlasGlyphInfo& info) {
                        m_atlasTexture = info.texture;

                        auto x0 = edt::CheckedCast<float>(currentX) + info.horizontalBearingX;
                        auto x1 = x0 + info.width;
                        auto y0 = edt::CheckedCast<float>(currentY) - edt::CheckedCast<float>(info.height - info.horizontalBearingY);
                        auto y1 = y0 + info.height;

                        auto tx0 = edt::CheckedCast<float>(info.x);
                        auto tx1 = tx0 + info.width;
                        auto ty0 = edt::CheckedCast<float>(info.y);
                        auto ty1 = ty0 + info.height;

                        Vertex q[4];
                        q[0].pos.rx() = x0;
                        q[0].pos.ry() = y0;
                        q[0].tex.rx() = tx0;
                        q[0].tex.ry() = ty1;
                        
                        q[1].pos.rx() = x1;
                        q[1].pos.ry() = y0;
                        q[1].tex.rx() = tx1;
                        q[1].tex.ry() = ty1;
                        
                        q[2].pos.rx() = x1;
                        q[2].pos.ry() = y1;
                        q[2].tex.rx() = tx1;
                        q[2].tex.ry() = ty0;
                        
                        q[3].pos.rx() = x0;
                        q[3].pos.ry() = y1;
                        q[3].tex.rx() = tx0;
                        q[3].tex.ry() = ty0;

                        {// Normalize coordinates
                            // Atlas size shortcuts
                            auto aw = info.texture->GetApiTexture()->GetWidth();
                            auto ah = info.texture->GetApiTexture()->GetHeight();

                            for (auto& v : q) {
                                v.pos.rx() /= w;
                                v.pos.ry() /= h;
                                v.pos.rz() = 1.0f;
                                v.pos.rw() = 1.0f;
                                v.tex.rx() /= aw;
                                v.tex.ry() /= ah;
                            }
                        }

                        vertices.push_back(q[1]);
                        vertices.push_back(q[0]);
                        vertices.push_back(q[3]);
                        vertices.push_back(q[2]);
                        vertices.push_back(q[1]);
                        vertices.push_back(q[3]);

                        currentX += edt::CheckedCast<int>(info.advanceX);
                    };

                    edt::Delegate<void(const AtlasGlyphInfo&)> delegate;
                    delegate.Bind(onRequest);
                    m_font->RequestGlyphsInfo(edt::MakeArrayView(unicodes), *m_graphicsSystem->GetDevice(), glyphParams, delegate);
                }

                PrimitiveBufferParameters params;
                params.usage = gpu::DeviceBufferUsage::Dynamic;
                params.bindFlags = gpu::DeviceBufferBindFlags::VertexBuffer;
                params.accessFlags = gpu::DeviceAccessFlags::Write;
                params.topology = gpu::PrimitiveTopology::TriangleList;
                m_textQuads.Initialize(*m_graphicsSystem, params, edt::MakeArrayView(vertices));
            }

            {// Create common constant buffer
                CB constantBufferInitData;
                edt::geom::Vector<float, 3> t {};
                constantBufferInitData.transform = MakeTranslationMatrix(t);

                gpu::DeviceBufferParameters params;
                params.size = sizeof(constantBufferInitData);
                params.usage = gpu::DeviceBufferUsage::Dynamic;
                params.bindFlags = gpu::DeviceBufferBindFlags::ConstantBuffer;
                params.accessFlags = gpu::DeviceAccessFlags::Write;
                m_constantBuffer = m_graphicsSystem->CreateDeviceBuffer(params, edt::DenseArrayView<uint8_t>((uint8_t*)&constantBufferInitData, sizeof(constantBufferInitData)));
            }

            {// Create container sampler
                gpu::SamplerParameters samplerParams{};
                samplerParams.addressU = gpu::TextureAddressMode::Clamp;
                samplerParams.addressV = gpu::TextureAddressMode::Clamp;
                samplerParams.addressW = gpu::TextureAddressMode::Clamp;
                samplerParams.filter = gpu::FilteringMode::Anisotropic;
                m_containerSampler = m_graphicsSystem->CreateSampler(samplerParams);
            }

            {// Create text sampler
                gpu::SamplerParameters samplerParams{};
                samplerParams.addressU = gpu::TextureAddressMode::Clamp;
                samplerParams.addressV = gpu::TextureAddressMode::Clamp;
                samplerParams.addressW = gpu::TextureAddressMode::Clamp;
                samplerParams.filter = gpu::FilteringMode::Bilinear;
                m_textSampler = m_graphicsSystem->CreateSampler(samplerParams);
            }

            LoadParameters(app);
        };
    }

    class SystemParams
    {
    public:
        void serialize(yasli::Archive& ar) {
            ar(vSync, "vSync");
        }

        bool vSync = false;
    };

    void System::LoadParameters(const keng::core::IApplicationPtr& app) {
        CallAndRethrowM + [&] {
            using namespace keng;
            using namespace filesystem;

            SystemParams params;
            try {
                filesystem::OpenFileParameters fileParams;
                fileParams.accessFlags = FileAccessFlags::Read;
                std::string filename = "Configs/";
                filename += GetSystemName();
                filename += ".json";
                auto buffer = ReadFileToBuffer(filename.data());
                yasli::JSONIArchive ar;
                OpenArchiveJSON(edt::DenseArrayView<uint8_t>(buffer.first.get(), buffer.second), ar);
                SerializeMandatory(ar, params, "");
            } catch (...) {}
            app->SetVSync(params.vSync);
        };
    }
}
