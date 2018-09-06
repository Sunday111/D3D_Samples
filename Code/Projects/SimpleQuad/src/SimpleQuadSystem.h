#pragma once

#include "Keng/Core/System.h"
#include "Keng/Graphics/IGraphicsSystem.h"
#include "Keng/WindowSystem/IWindowSystem.h"
#include "Keng/ResourceSystem/IResourceSystem.h"

namespace simple_quad_sample
{
    class SimpleQuadSystem : public keng::core::System<
            keng::core::ISystem, SimpleQuadSystem,
            keng::resource::IResourceSystem,
            keng::graphics::IGraphicsSystem,
            keng::window_system::IWindowSystem>
    {
    public:
        SimpleQuadSystem();
        ~SimpleQuadSystem();

        // ISystem
        static const char* SystemName() { return "SimpleQuadSystem"; }
        virtual void OnSystemEvent(const keng::core::IApplicationPtr&, const keng::core::SystemEvent& e) override;

    private:
        void Initialize(const keng::core::IApplicationPtr& app);
        bool Update();
        void Shutdown();

    private:
        keng::graphics::IEffectPtr m_effect;
        keng::graphics::gpu::IDeviceBufferPtr m_constantBuffer;
        keng::graphics::gpu::IDeviceBufferPtr m_vertexBuffer;
        keng::graphics::gpu::ITextureRenderTargetPtr m_textureRT;
        keng::graphics::gpu::IDepthStencilPtr m_depthStencil;
        keng::graphics::gpu::IWindowRenderTargetPtr m_windowRT;
        keng::graphics::gpu::IAnnotationPtr m_annotation;
    };
}
