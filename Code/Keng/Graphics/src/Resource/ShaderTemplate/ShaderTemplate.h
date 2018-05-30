#pragma once

#include "FwdDecl.h"
#include "Resource/Shader/Shader.h"

namespace keng::graphics
{
    class ShaderTemplate :
        public core::RefCountImpl<resource::IResource>
    {
    public:
        ShaderTemplate();

        graphics_api::ShaderType type;
        std::string code;
    };
}
