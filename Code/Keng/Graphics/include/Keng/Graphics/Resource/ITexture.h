#pragma once

#include "EverydayTools\Exception\CallAndRethrow.h"
#include "Keng/ResourceSystem/IResource.h"
#include "Keng/Core/Ptr.h"

#include <vector>

#include "Keng/Graphics/FragmentFormat.h"
#include "Keng/Graphics/ShaderType.h"

namespace keng::graphics
{
    class Device;

    class ITexture : public resource::IResource
    {
    public:
        virtual void* GetNativeInterface() const = 0;
        virtual FragmentFormat GetFormat() const = 0;
        virtual void AssignToPipeline(ShaderType shaderType, size_t slot) = 0;
        virtual void CopyTo(core::Ptr<ITexture>) = 0;
        virtual ~ITexture() = default;
    };
}
