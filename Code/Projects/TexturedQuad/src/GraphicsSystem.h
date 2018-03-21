#pragma once

#include "Keng/Graphics/GraphicsSystem.h"

namespace textured_quad_sample
{
	class GraphicsSystem : public keng::graphics::GraphicsSystem
	{
	public:
		using Base = keng::graphics::GraphicsSystem;
        virtual void Initialize(keng::core::IApplication* app) override;
        virtual bool Update() override;

	protected:
		Base::ShaderInfo m_drawShader;
		ComPtr<ID3D11SamplerState> m_sampler;
		std::unique_ptr<d3d_tools::ICrossDeviceBuffer> m_buffer;
		std::shared_ptr<keng::graphics::Texture> m_texture;
		std::shared_ptr<keng::graphics::TextureView<keng::graphics::ResourceViewType::ShaderResource>> m_textureView;
	};
}
