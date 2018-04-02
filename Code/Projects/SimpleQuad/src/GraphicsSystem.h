#pragma once

#include "Keng/Graphics/GraphicsSystem.h"

namespace simple_quad_sample
{
    class GraphicsSystem : public keng::graphics::GraphicsSystem
    {
    public:
        GraphicsSystem();
        ~GraphicsSystem();

        using Base = keng::graphics::GraphicsSystem;
        virtual void Initialize(keng::core::IApplication* app) override;
        virtual bool Update() override;

    protected:
        keng::graphics::IEffectPtr m_effect;
        keng::graphics::IDeviceBufferPtr m_constantBuffer;
        keng::graphics::IDeviceBufferPtr m_vertexBuffer;
        keng::graphics::ITextureRenderTargetPtr m_textureRT;
        keng::graphics::IDepthStencilPtr m_depthStencil;
        keng::graphics::IWindowRenderTargetPtr m_windowRT;
    };
}
