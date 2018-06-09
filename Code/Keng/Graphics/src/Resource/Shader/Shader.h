#pragma once

#include "FwdDecl.h"
#include "EverydayTools/Array/ArrayView.h"
#include "Keng/GPU/Shader/IDeviceShader.h"
#include "Keng/ResourceSystem/IResourceSystem.h"
#include "Keng/ResourceSystem/IResource.h"
#include "WinWrappers/ComPtr.h"

namespace keng::graphics
{
    template<ShaderType shaderType>
    class Shader : public core::RefCountImpl<resource::IResource>
    {
    public:
        core::Ptr<gpu::IShaderT<shaderType>> m_impl;
    };
}
