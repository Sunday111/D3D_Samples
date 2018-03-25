#pragma once

#include "Keng/Graphics/Resource/IEffect.h"
#include "Resource/Shader/Shader.h"

namespace keng::graphics
{
	class Effect : public IEffect
	{
        IMPLEMENT_IREFCOUNT
	public:
        // IEffect
        virtual void InitDefaultInputLayout(IDevice* device) override;
        virtual void Use(IDevice* device) override;

        ComPtr<ID3D11InputLayout> layout;
		core::Ptr<Shader<ShaderType::Vertex>> vs;
		core::Ptr<Shader<ShaderType::Pixel>> fs;
	};

}
