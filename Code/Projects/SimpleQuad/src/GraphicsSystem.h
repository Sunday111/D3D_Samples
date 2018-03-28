#pragma once

#include "Keng/Graphics/GraphicsSystem.h"

namespace simple_quad_sample
{
    class GraphicsSystem : public keng::graphics::GraphicsSystem
    {
    public:
        using Base = keng::graphics::GraphicsSystem;
        virtual void Initialize(keng::core::IApplication* app) override;
        virtual bool Update() override;

    protected:
        keng::core::Ptr<keng::graphics::IEffect> m_effect;
        keng::core::Ptr<keng::graphics::IDeviceBuffer> m_constantBuffer;
        keng::core::Ptr<keng::graphics::IDeviceBuffer> m_vertexBuffer;
    };
}
