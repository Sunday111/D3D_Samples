#pragma once

#include "Keng/Graphics/FwdDecl.h"
#include "Keng/Core/ISystem.h"

namespace textured_quad_sample
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
        keng::graphics::IDeviceBufferPtr m_constantBuffer;
        keng::graphics::IDeviceBufferPtr m_vertexBuffer;
        keng::graphics::ITextureRenderTargetPtr m_textureRT;
        keng::graphics::ITexturePtr m_texture;
        keng::graphics::IDepthStencilPtr m_depthStencil;
        keng::graphics::IWindowRenderTargetPtr m_windowRT;
        keng::graphics::ISamplerPtr m_sampler;
        keng::graphics::IAnnotationPtr m_annotation;

        keng::resource::IResourceSystemPtr m_resourceSystem;
        keng::graphics::IGraphicsSystemPtr m_graphicsSystem;
        keng::window_system::IWindowSystemPtr m_windowSystem;
    };
}
