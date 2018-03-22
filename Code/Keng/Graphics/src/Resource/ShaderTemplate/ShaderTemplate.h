#pragma once

#include "Resource/Shader/Shader.h"

namespace keng::graphics
{
	class ShaderTemplate :
        public resource::IResource
	{
	public:
        ShaderTemplate();

		ShaderType type;
		std::string code;
	};
}
