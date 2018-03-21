#pragma once

#include "Keng/Graphics/GraphicsSystem.h"

namespace simple_quad_sample
{
	class GraphicsSystem : public keng::GraphicsSystem
	{
	public:
		using Base = keng::GraphicsSystem;
        virtual void Initialize(keng::IApplication* app) override;
        virtual bool Update() override;

	protected:
		Base::ShaderInfo m_drawShader;
		std::unique_ptr<d3d_tools::ICrossDeviceBuffer> m_buffer;
	};
}
