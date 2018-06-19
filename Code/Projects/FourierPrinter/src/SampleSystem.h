#pragma once

#include "Keng/Graphics/FwdDecl.h"
#include "Keng/Core/ISystem.h"

namespace simple_quad_sample
{
    class VertexBuffer
    {
    public:
        size_t elementsCount = 0;
        keng::graphics::PrimitiveTopology topology;
        keng::graphics::IEffectPtr effect;
        keng::graphics::gpu::IDeviceBufferPtr vertices;
        keng::graphics::gpu::IDeviceBufferPtr m_constantBuffer;
    };

    class SampleSystem : public keng::core::RefCountImpl<keng::core::ISystem>
    {
    public:
        SampleSystem();
        ~SampleSystem();

        // ISystem
        virtual void Initialize(const keng::core::IApplicationPtr& app) override;
        virtual bool Update() override;
        virtual void Shutdown() override;
        virtual const char* GetSystemName() const override;
        virtual bool ForEachDependency(const edt::Delegate<bool(const char*)>& delegate) const override;

    protected:
        VertexBuffer m_vertexBuffer;
        keng::graphics::gpu::IDeviceBufferPtr m_constantBuffer;
        keng::graphics::gpu::ITextureRenderTargetPtr m_textureRT;
        keng::graphics::gpu::IDepthStencilPtr m_depthStencil;
        keng::graphics::gpu::IWindowRenderTargetPtr m_windowRT;
        keng::graphics::gpu::IAnnotationPtr m_annotation;

        keng::resource::IResourceSystemPtr m_resourceSystem;
        keng::graphics::IGraphicsSystemPtr m_graphicsSystem;
        keng::window_system::IWindowSystemPtr m_windowSystem;
    };
}
