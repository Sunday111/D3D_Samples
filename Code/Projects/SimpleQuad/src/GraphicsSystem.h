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
		Base::ShaderInfo m_drawShader;
		std::unique_ptr<d3d_tools::ICrossDeviceBuffer> m_buffer;
	};
}
