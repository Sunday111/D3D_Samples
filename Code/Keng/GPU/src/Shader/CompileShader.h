#pragma once

#include "Keng/GPU/Shader/ShaderParameters.h"
#include "d3d11_1.h"
#include "WinWrappers/ComPtr.h"
#include "ShaderVersion.h"

namespace keng::gpu
{
    ComPtr<ID3DBlob> CompileShaderToBlob(ShaderType shaderType, ShaderVersion shaderVersion, const ShaderParameters& parameters);
}
