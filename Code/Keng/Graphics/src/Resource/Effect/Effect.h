#pragma once

#include "Keng/Graphics/Resource/IEffect.h"
#include "Resource/Shader/Shader.h"

namespace keng::graphics
{
    class Device;

	class Effect : public IEffect
	{
        IMPLEMENT_IREFCOUNT
	public:
        Effect();
        ~Effect();

        // IEffect
        virtual void InitDefaultInputLayout() override;
        virtual void Use() override;

        core::Ptr<Device> device;
        ComPtr<ID3D11InputLayout> layout;
		core::Ptr<Shader<ShaderType::Vertex>> vs;
		core::Ptr<Shader<ShaderType::Pixel>> fs;
	};

}
