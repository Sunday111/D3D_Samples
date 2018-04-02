#pragma once

#include "Keng/Graphics/FwdDecl.h"
#include "Keng/Core/ISystem.h"

namespace simple_quad_sample
{
    class SampleSystem : public keng::core::ISystem
    {
    public:
        SampleSystem();
        ~SampleSystem();

        // ISystem
        virtual void Initialize(keng::core::IApplication* app) override;
        virtual bool Update() override;
        virtual const char* GetSystemGUID() override;
        virtual bool ForEachSystemDependency(bool(*pfn)(const char* systemGUID, void* context), void* context) override;

    protected:
        keng::graphics::IGraphicsSystem* m_graphicsSystem;
        keng::graphics::IEffectPtr m_effect;
        keng::graphics::IDeviceBufferPtr m_constantBuffer;
        keng::graphics::IDeviceBufferPtr m_vertexBuffer;
        keng::graphics::ITextureRenderTargetPtr m_textureRT;
        keng::graphics::IDepthStencilPtr m_depthStencil;
        keng::graphics::IWindowRenderTargetPtr m_windowRT;
        keng::graphics::IAnnotationPtr m_annotation;
    };
}
