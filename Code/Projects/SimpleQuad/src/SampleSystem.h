#pragma once

#include "Keng/Graphics/FwdDecl.h"
#include "Keng/Core/ISystem.h"

namespace simple_quad_sample
{
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
        keng::graphics::IEffectPtr m_effect;
        keng::gpu::IDeviceBufferPtr m_constantBuffer;
        keng::gpu::IDeviceBufferPtr m_vertexBuffer;
        keng::gpu::ITextureRenderTargetPtr m_textureRT;
        keng::gpu::IDepthStencilPtr m_depthStencil;
        keng::gpu::IWindowRenderTargetPtr m_windowRT;
        keng::gpu::IAnnotationPtr m_annotation;

        keng::resource::IResourceSystemPtr m_resourceSystem;
        keng::graphics::IGraphicsSystemPtr m_graphicsSystem;
        keng::window_system::IWindowSystemPtr m_windowSystem;
    };
}
