#pragma once

#include "Keng/Graphics/Resource/IEffect.h"
#include "Resource/Shader/Shader.h"

namespace keng::graphics
{
	class Effect : public IEffect
	{
	public:
        // IEffect
        virtual void InitDefaultInputLayout(IDevice* device) override;
        virtual void Use(IDevice* device) override;

        ComPtr<ID3D11InputLayout> layout;
		std::shared_ptr<Shader<ShaderType::Vertex>> vs;
		std::shared_ptr<Shader<ShaderType::Pixel>> fs;
	};

}
