#pragma once

#include "Keng/Graphics/FragmentFormat.h"
#include "Keng/Graphics/Resource/ITexture.h"

namespace keng::graphics
{
    class DepthStencilParameters
    {
    public:
        FragmentFormat format;
        ITexturePtr texture;
    };
}
