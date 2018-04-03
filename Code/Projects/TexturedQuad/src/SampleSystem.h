#pragma once

#include "Keng/Graphics/FwdDecl.h"
#include "Keng/Core/ISystem.h"

namespace textured_quad_sample
{
    class SampleSystem : public keng::core::ISystem
    {
    public:
        SampleSystem();
        ~SampleSystem();

        // ISystem
        virtual void Initialize(keng::core::IApplication* app) override;
        virtual bool Update() override;
        virtual std::string_view GetSystemName() const override;
        virtual bool ForEachSystemDependency(bool(*pfn)(std::string_view systemName, void* context), void* context) const override;

    protected:
        keng::resource::IResourceSystem* m_resourceSystem = nullptr;
        keng::graphics::IGraphicsSystem* m_graphicsSystem = nullptr;
        keng::window_system::IWindowSystem* m_windowSystem = nullptr;

        keng::graphics::IEffectPtr m_effect;
        keng::graphics::IDeviceBufferPtr m_constantBuffer;
        keng::graphics::IDeviceBufferPtr m_vertexBuffer;
        keng::graphics::ITextureRenderTargetPtr m_textureRT;
        keng::graphics::ITexturePtr m_texture;
        keng::graphics::IDepthStencilPtr m_depthStencil;
        keng::graphics::IWindowRenderTargetPtr m_windowRT;
        keng::graphics::ISamplerPtr m_sampler;
        keng::graphics::IAnnotationPtr m_annotation;
    };
}
