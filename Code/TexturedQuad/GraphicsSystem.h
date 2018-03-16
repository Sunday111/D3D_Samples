#pragma once

#include "Keng/GraphicsSystem.h"

namespace textured_quad_sample
{
	class GraphicsSystem : public keng::GraphicsSystem
	{
	public:
		using Base = keng::GraphicsSystem;
		using CreateParams = keng::GraphicsSystem::CreateParams;

		virtual void Initialize(const CreateParams& params, IApplication* app) override;
		virtual bool Update() override;

	protected:
		Base::ShaderInfo m_drawShader;
		ComPtr<ID3D11SamplerState> m_sampler;
		std::unique_ptr<d3d_tools::ICrossDeviceBuffer> m_buffer;
		IntrusivePtr<keng::Texture> m_texture;
		IntrusivePtr<keng::TextureView<keng::ResourceViewType::ShaderResource>> m_textureView;
	};
}