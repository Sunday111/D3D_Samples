#pragma once

#include "Keng/Graphics/FwdDecl.h"
#include "Keng/ResourceSystem/IResource.h"

namespace keng::graphics
{
    class IFont : public resource::IResource
    {
    public:
        virtual void Do(IDevice& device) = 0;
        virtual ITexturePtr GetTexture(IDevice& device) const = 0;
        virtual ~IFont() = default;
    };
}

