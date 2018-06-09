#pragma once

#include "Keng/GPU/Shader/ShaderType.h"

namespace keng::graphics::gpu
{
    enum class ShaderVersion
    {
        _5_0,
        _4_1,
        _4_0,
    };

    const char* ShaderTypeToShaderTarget(ShaderType shaderType, ShaderVersion shaderVersion);
}
