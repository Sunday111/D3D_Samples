#pragma once

#include "Keng/Graphics/GraphicsSystem.h"

namespace keng::graphics {
    class IEffect;
}

namespace textured_quad_sample
{
	class GraphicsSystem : public keng::graphics::GraphicsSystem
	{
	public:
		using Base = keng::graphics::GraphicsSystem;
        virtual void Initialize(keng::core::IApplication* app) override;
        virtual bool Update() override;

	protected:
        keng::core::Ptr<keng::graphics::IEffect> m_effect;
		ComPtr<ID3D11SamplerState> m_sampler;
		std::unique_ptr<d3d_tools::ICrossDeviceBuffer> m_buffer;
		keng::core::Ptr<keng::graphics::Texture> m_texture;
		keng::core::Ptr<keng::graphics::TextureView<keng::graphics::ResourceViewType::ShaderResource>> m_textureView;
	};
}
