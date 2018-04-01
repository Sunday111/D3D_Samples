#include "GraphicsSystem.h"

#include "Keng/Core/Application.h"
#include "Keng/Graphics/DeviceBufferMapper.h"
#include "Keng/Graphics/Resource/ITexture.h"
#include "Keng/Graphics/Resource/TextureView.h"
#include "Keng/Graphics/Resource/IEffect.h"
#include "Keng/Graphics/RenderTarget/IWindowRenderTarget.h"
#include "Keng/Graphics/RenderTarget/ITextureRenderTarget.h"
#include "Keng/Graphics/RenderTarget/IDepthStencil.h"
#include "Keng/ResourceSystem/IResourceSystem.h"

#include "D3D_Tools/Annotation.h"
#include "EverydayTools/Geom/Vector.h"
#include "EverydayTools/Array/ArrayViewVector.h"
#include "Keng/Graphics/IDeviceBuffer.h"

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

    GraphicsSystem::GraphicsSystem() = default;
    GraphicsSystem::~GraphicsSystem() = default;

    bool GraphicsSystem::Update() {
        using namespace keng;
        using namespace graphics;

        return CallAndRethrowM + [&] {
            return d3d_tools::Annotate(m_device.Get(), L"Frame", [&]() {
                float clearColor[4]{
                    0.0f, 0.2f, 0.4f, 1.0f
                };

                static float angle = 0.f;
                constexpr float delta_angle = 0.001f;
                angle += delta_angle;

                d3d_tools::Annotate(m_device.Get(), L"Move triangle", [&]() {
                    DeviceBufferMapper mapper;
                    m_constantBuffer->MakeMapper(mapper);
                    auto cbView = mapper.GetTypedView<CB>();
                    edt::geom::Vector<float, 3> t{};
                    t.rx() = std::sin(angle);
                    cbView[0].transform = MakeTranslationMatrix(t);
                });

                d3d_tools::Annotate(m_device.Get(), L"Draw triangle", [&]() {
                    VertexBufferAssignParameters vbAssignParams{};
                    vbAssignParams.slot = 0;
                    vbAssignParams.stride = sizeof(Vertex);

                    ConstantBufferAssignParameters cbAssignParams{};
                    cbAssignParams.slot = 0;
                    cbAssignParams.stride = sizeof(CB);
                    cbAssignParams.shaderType = ShaderType::Vertex;

                    m_textureRT->Clear(clearColor);
                    m_depthStencil->Clear(DepthStencilClearFlags::ClearDepth | DepthStencilClearFlags::ClearStencil, 1.0f, 0);
                    m_textureRT->AssignToPipeline(m_depthStencil);
                    m_effect->AssignToPipeline();
                    m_vertexBuffer->AssignToPipeline(vbAssignParams);
                    m_device->SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
                    m_constantBuffer->AssignToPipeline(cbAssignParams);
                    m_device->SetSampler(0, m_sampler.Get(), d3d_tools::ShaderType::Pixel);
                    m_texture->AssignToPipeline(ShaderType::Fragment, 0);
                    m_device->Draw(4);
                });

                d3d_tools::Annotate(m_device.Get(), L"Copy texture to swap chain texture", [&]() {
                    GetWindowRenderTarget()->CopyFrom(m_textureRT->GetTexture());
                });

                GetWindowRenderTarget()->Present();

                return true;
            });
        };
    }

    void GraphicsSystem::Initialize(keng::core::IApplication* app) {
        CallAndRethrowM + [&] {
            Base::Initialize(app);
            using namespace keng;
            using namespace keng::resource;
            using namespace keng::graphics;
            auto resourceSystem = dynamic_cast<core::Application*>(app)->GetSystem<IResourceSystem>();
            m_texture = std::static_pointer_cast<ITexture>(resourceSystem->GetResource("Assets/Textures/container.xml"));

            {// Read and compile shaders
                std::string_view effectName = "Assets/Effects/Textured.xml";
                m_effect = std::static_pointer_cast<IEffect>(resourceSystem->GetResource(effectName));
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

                graphics::DeviceBufferParams params;
                params.size = sizeof(vertices);
                params.usage = graphics::DeviceBufferUsage::Dynamic;
                params.bindFlags = graphics::DeviceBufferBindFlags::VertexBuffer;
                params.accessFlags = graphics::DeviceAccessFlags::Write;
                m_vertexBuffer = CreateDeviceBuffer(params, edt::DenseArrayView<uint8_t>((uint8_t*)&vertices, sizeof(vertices)));
            }

            {
                CB constantBufferInitData;
                edt::geom::Vector<float, 3> t{};
                constantBufferInitData.transform = MakeTranslationMatrix(t);

                graphics::DeviceBufferParams params;
                params.size = sizeof(constantBufferInitData);
                params.usage = graphics::DeviceBufferUsage::Dynamic;
                params.bindFlags = graphics::DeviceBufferBindFlags::ConstantBuffer;
                params.accessFlags = graphics::DeviceAccessFlags::Write;
                m_constantBuffer = CreateDeviceBuffer(params, edt::DenseArrayView<uint8_t>((uint8_t*)&constantBufferInitData, sizeof(constantBufferInitData)));
            }

            {// Create sampler
                CD3D11_SAMPLER_DESC samplerDesc(D3D11_DEFAULT);
                samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
                samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
                m_device->GetDevice()->CreateSamplerState(&samplerDesc, m_sampler.Receive());
            }
        };
    }

}
