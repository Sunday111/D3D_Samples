#pragma once

#include "Keng/Graphics/FwdDecl.h"
#include "Keng/ResourceSystem/IResource.h"
#include "Keng/GraphicsAPI/Resource/ITexture.h"

namespace keng::graphics
{
    class ITexture : public resource::IResource
    {
    public:
        virtual graphics_api::ITexturePtr GetApiTexture() = 0;
        virtual ~ITexture() = default;
    };
}
