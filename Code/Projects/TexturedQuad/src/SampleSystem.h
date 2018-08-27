#pragma once

#include "Keng/Core/System.h"
#include "Keng/Graphics/IGraphicsSystem.h"
#include "Keng/WindowSystem/IWindowSystem.h"
#include "Keng/ResourceSystem/IResourceSystem.h"

namespace textured_quad_sample
{
    class SampleSystem : public keng::core::System<
            keng::core::ISystem, SampleSystem,
            keng::resource::IResourceSystem,
            keng::graphics::IGraphicsSystem,
            keng::window_system::IWindowSystem>
    {
    public:
        SampleSystem();
        ~SampleSystem();

        // ISystem
        static const char* SystemName() { return "TexturedQuadSystem"; }
        virtual void Initialize(const keng::core::IApplicationPtr& app) override;
        virtual bool Update() override;
        virtual void Shutdown() override;

    protected:
        keng::graphics::IEffectPtr m_effect;
        keng::graphics::gpu::IDeviceBufferPtr m_constantBuffer;
        keng::graphics::gpu::IDeviceBufferPtr m_vertexBuffer;
        keng::graphics::gpu::ITextureRenderTargetPtr m_textureRT;
        keng::graphics::ITexturePtr m_texture;
        keng::graphics::gpu::IDepthStencilPtr m_depthStencil;
        keng::graphics::gpu::IWindowRenderTargetPtr m_windowRT;
        keng::graphics::gpu::ISamplerPtr m_sampler;
        keng::graphics::gpu::IAnnotationPtr m_annotation;
    };
}
