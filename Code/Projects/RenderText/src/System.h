#pragma once

#include "EverydayTools/Geom/Vector.h"
#include "Keng/Graphics/FwdDecl.h"
#include "Keng/Core/ISystem.h"
#include "Keng/Graphics/PrimitiveTopology.h"
#include "Keng/Graphics/IDeviceBuffer.h"
#include "Keng/Graphics/IGraphicsSystem.h"

namespace render_text_sample
{
    struct Vertex
    {
        edt::geom::Vector<float, 4> pos;
        edt::geom::Vector<float, 2> tex;
    };

    class PrimitiveBufferParameters
    {
    public:
        keng::graphics::PrimitiveTopology topology = keng::graphics::PrimitiveTopology::TriangleList;
        keng::graphics::DeviceBufferUsage usage = keng::graphics::DeviceBufferUsage::Default;
        keng::graphics::DeviceBufferBindFlags bindFlags = keng::graphics::DeviceBufferBindFlags::None;
        keng::graphics::DeviceAccessFlags accessFlags = keng::graphics::DeviceAccessFlags::None;
    };

    template<typename T>
    class PrimitiveBuffer
    {
    public:
        void Initialize(keng::graphics::IGraphicsSystem& system, const PrimitiveBufferParameters& params, edt::DenseArrayView<const T> data) {
            using namespace keng;
            using namespace graphics;

            m_topo = params.topology;
            m_verticesCount = data.GetSize();

            DeviceBufferParams deviceBufferParams;
            deviceBufferParams.size = GetBytesCount();
            deviceBufferParams.usage = params.usage;
            deviceBufferParams.bindFlags = params.bindFlags;
            deviceBufferParams.accessFlags = params.accessFlags;
            m_vertices = system.CreateDeviceBuffer(deviceBufferParams,
                edt::DenseArrayView<uint8_t>((uint8_t*)data.GetData(), deviceBufferParams.size));
        }

        void AssignToPipeline(size_t slot, keng::graphics::IGraphicsSystemPtr system) {
            keng::graphics::VertexBufferAssignParameters vbAssignParams{};
            vbAssignParams.slot = 0;
            vbAssignParams.stride = sizeof(T);
            m_vertices->AssignToPipeline(vbAssignParams);
            system->SetTopology(m_topo);
        }

        size_t GetVerticesCount() const {
            return m_verticesCount;
        }

        size_t GetBytesCount() const {
            return m_verticesCount * GetStride();
        }

        size_t GetStride() const {
            return sizeof(T);
        }

    private:
        size_t m_verticesCount;
        keng::graphics::PrimitiveTopology m_topo;
        keng::graphics::IDeviceBufferPtr m_vertices;
    };

    class System : public keng::core::RefCountImpl<keng::core::ISystem>
    {
    public:
        System();
        ~System();

        // ISystem
        virtual void Initialize(const keng::core::IApplicationPtr& app) override;
        virtual bool Update() override;
        virtual void Shutdown() override;
        virtual const char* GetSystemName() const override;
        virtual bool ForEachDependency(const edt::Delegate<bool(const char*)>& delegate) const override;

    protected:
        void LoadParameters(const keng::core::IApplicationPtr& app);

        keng::graphics::IDeviceBufferPtr m_constantBuffer;

        keng::graphics::IEffectPtr m_texturedEffect;
        PrimitiveBuffer<Vertex> m_containerQuad;
        keng::graphics::ITexturePtr m_containerTexture;
        keng::graphics::ISamplerPtr m_containerSampler;

        keng::graphics::IEffectPtr m_textEffect;
        PrimitiveBuffer<Vertex> m_textQuads;
        keng::graphics::ITexturePtr m_atlasTexture;
        keng::graphics::ISamplerPtr m_textSampler;
        keng::graphics::IFontPtr m_font;

        keng::graphics::IDepthStencilPtr m_depthStencil;
        keng::graphics::IWindowRenderTargetPtr m_windowRT;
        keng::graphics::IAnnotationPtr m_annotation;

        keng::resource::IResourceSystemPtr m_resourceSystem;
        keng::graphics::IGraphicsSystemPtr m_graphicsSystem;
        keng::window_system::IWindowSystemPtr m_windowSystem;
    };
}
