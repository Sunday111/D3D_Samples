#pragma once

#include "d3d11_1.h"
#include "WinWrappers/ComPtr.h"
#include "ShaderVersion.h"
#include "FwdDecl.h"

namespace keng::graphics::gpu
{
    ComPtr<ID3DBlob> CompileShaderToBlob(const ShaderType& shaderType, ShaderVersion shaderVersion, const DeviceShaderParameters& parameters);
}
