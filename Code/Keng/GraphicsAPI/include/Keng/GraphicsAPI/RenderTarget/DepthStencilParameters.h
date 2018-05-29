#pragma once

#include "Keng/Graphics/FragmentFormat.h"
#include "Keng/Graphics/Resource/ITexture.h"

namespace keng::graphics_api
{
    class DepthStencilParameters
    {
    public:
        FragmentFormat format;
        ITexturePtr texture;
    };
}
