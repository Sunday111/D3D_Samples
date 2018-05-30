#pragma once

#include "Keng/GPU/FragmentFormat.h"
#include "Keng/GPU/Resource/ITexture.h"

namespace keng::gpu
{
    class DepthStencilParameters
    {
    public:
        FragmentFormat format;
        ITexturePtr texture;
    };
}
