#pragma once

#include "Keng/ResourceSystem/IResourceSystem.h"
#include "Keng/ResourceSystem/IResource.h"
#include "D3D_Tools/Shader.h"

namespace keng::graphics
{
    class Device;
    using ShaderType = d3d_tools::ShaderType;
    using ShaderVersion = d3d_tools::ShaderVersion;

    template<ShaderType shaderType>
    class Shader :
        public resource::IResource
    {
        IMPLEMENT_IREFCOUNT
    public:
        d3d_tools::Shader<shaderType> m_impl;
    };
}
