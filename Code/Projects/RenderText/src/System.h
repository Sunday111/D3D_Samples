#pragma once

#include "EverydayTools/Geom/Vector.h"

#include "Keng/Core/System.h"
#include "Keng/ResourceSystem/IResourceSystem.h"
#include "Keng/FileSystem/IFileSystem.h"
#include "Keng/GPU/Resource/IDeviceBuffer.h"
#include "Keng/Graphics/IGraphicsSystem.h"
#include "Keng/GraphicsCommon/DeviceBufferParameters.h"
#include "Keng/GraphicsCommon/PrimitiveTopology.h"
#include "Keng/WindowSystem/IWindowSystem.h"

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
        keng::graphics::CpuAccessFlags accessFlags = keng::graphics::CpuAccessFlags::None;
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

            DeviceBufferParameters deviceBufferParams{};
            deviceBufferParams.size = GetBytesCount();
            deviceBufferParams.usage = params.usage;
            deviceBufferParams.bindFlags = params.bindFlags;
            deviceBufferParams.accessFlags = params.accessFlags;
            m_vertices = system.GetDevice()->GetApiDevice()->CreateDeviceBuffer(deviceBufferParams,
                edt::DenseArrayView<uint8_t>((uint8_t*)data.GetData(), deviceBufferParams.size));
        }

        void AssignToPipeline(size_t slot, keng::graphics::IGraphicsSystem& system) {
            keng::graphics::gpu::VertexBufferAssignParameters vbAssignParams{};
            vbAssignParams.slot = 0;
            vbAssignParams.stride = sizeof(T);
            m_vertices->AssignToPipeline(vbAssignParams);
            system.GetDevice()->GetApiDevice()->SetTopology(m_topo);
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
        keng::graphics::gpu::IDeviceBufferPtr m_vertices;
    };

    class System : public keng::core::System<
            keng::core::ISystem, render_text_sample::System,
            keng::filesystem::IFileSystem,
            keng::resource::IResourceSystem,
            keng::graphics::IGraphicsSystem,
            keng::window_system::IWindowSystem>
    {
    public:
        System();
        ~System();

        // ISystem
        static const char* SystemName() { return "RenderTextSample"; }
        virtual void Initialize(const keng::core::IApplicationPtr& app) override;
        virtual bool Update() override;
        virtual void Shutdown() override;

    protected:
        void LoadParameters(const keng::core::IApplicationPtr& app);

        keng::graphics::gpu::IDeviceBufferPtr m_constantBuffer;

        keng::graphics::IEffectPtr m_texturedEffect;
        PrimitiveBuffer<Vertex> m_containerQuad;
        keng::graphics::ITexturePtr m_containerTexture;
        keng::graphics::gpu::ISamplerPtr m_containerSampler;

        keng::graphics::IEffectPtr m_textEffect;
        PrimitiveBuffer<Vertex> m_textQuads;
        keng::graphics::ITexturePtr m_atlasTexture;
        keng::graphics::gpu::ISamplerPtr m_textSampler;
        keng::graphics::IFontPtr m_font;

        keng::graphics::gpu::IDepthStencilPtr m_depthStencil;
        keng::graphics::gpu::IWindowRenderTargetPtr m_windowRT;
        keng::graphics::gpu::IAnnotationPtr m_annotation;
    };
}
