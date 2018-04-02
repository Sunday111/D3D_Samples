#pragma once

#include "Keng/Graphics/FwdDecl.h"
#include "Keng/Graphics/FragmentFormat.h"
#include "Keng/Graphics/ShaderType.h"
#include "Keng/ResourceSystem/IResource.h"

namespace keng::graphics
{
    class ITexture : public resource::IResource
    {
    public:
        virtual void* GetNativeInterface() const = 0;
        virtual FragmentFormat GetFormat() const = 0;
        virtual void AssignToPipeline(ShaderType shaderType, size_t slot) = 0;
        virtual void CopyTo(ITexturePtr) = 0;
        virtual ~ITexture() = default;
    };
}
