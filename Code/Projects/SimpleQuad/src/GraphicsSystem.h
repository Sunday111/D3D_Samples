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
        std::shared_ptr<keng::graphics::IEffect> m_effect;
		std::unique_ptr<d3d_tools::ICrossDeviceBuffer> m_buffer;
	};
}
