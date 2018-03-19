#pragma once

#include "Keng/Systems/GraphicsSystem/GraphicsSystem.h"

namespace simple_quad_sample
{
	class GraphicsSystem : public keng::GraphicsSystem
	{
	public:
		using Base = keng::GraphicsSystem;
		using CreateParams = keng::GraphicsSystem::CreateParams;

		virtual void Initialize(const CreateParams& params) override;
		virtual bool Update() override;

	protected:
		Base::ShaderInfo m_drawShader;
		std::unique_ptr<d3d_tools::ICrossDeviceBuffer> m_buffer;
	};
}
