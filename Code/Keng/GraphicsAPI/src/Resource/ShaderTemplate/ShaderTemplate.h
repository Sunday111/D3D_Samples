#pragma once

#include "FwdDecl.h"
#include "Resource/Shader/Shader.h"

namespace keng::graphics_api
{
    class ShaderTemplate :
        public core::RefCountImpl<core::IRefCountObject>
    {
    public:
        ShaderTemplate();

        ShaderType type;
        std::string code;
    };
}
