#pragma once

#include "D3D_Tools\Shader.h"

class Device;

using ShaderType = d3d_tools::ShaderType;
using ShaderVersion = d3d_tools::ShaderVersion;

template<ShaderType shaderType>
using Shader = d3d_tools::Shader<shaderType>;