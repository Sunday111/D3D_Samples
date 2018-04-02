#pragma once

#include "FwdDecl.h"
#include "Keng/ResourceSystem/IResourceSystem.h"
#include "Keng/ResourceSystem/IResource.h"
#include "D3D_Tools/Shader.h"
#include "Keng/Graphics/ShaderType.h"

namespace keng::graphics
{
    using ShaderVersion = d3d_tools::ShaderVersion;

    template<d3d_tools::ShaderType shaderType>
    class Shader : public core::RefCountImpl<resource::IResource>
    {
    public:
        d3d_tools::Shader<shaderType> m_impl;
    };
}
