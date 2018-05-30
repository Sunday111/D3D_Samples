#pragma once

#include "FwdDecl.h"
#include "EverydayTools/Array/ArrayView.h"
#include "Keng/GraphicsAPI/Shader/IShader.h"
#include "Keng/ResourceSystem/IResourceSystem.h"
#include "Keng/ResourceSystem/IResource.h"
#include "WinWrappers/ComPtr.h"

namespace keng::graphics
{
    template<graphics_api::ShaderType shaderType>
    class Shader : public core::RefCountImpl<resource::IResource>
    {
    public:
        core::Ptr<graphics_api::IShaderT<shaderType>> m_impl;
    };
}
