#pragma once

#include "Keng/Graphics/GraphicsSystem.h"

namespace keng::graphics
{
    class IEffect;
}

namespace textured_quad_sample
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
        ComPtr<ID3D11SamplerState> m_sampler;
        keng::core::Ptr<keng::graphics::IEffect> m_effect;
        keng::core::Ptr<keng::graphics::IDeviceBuffer> m_constantBuffer;
        keng::core::Ptr<keng::graphics::IDeviceBuffer> m_vertexBuffer;
        keng::core::Ptr<keng::graphics::ITexture> m_texture;
    };
}
