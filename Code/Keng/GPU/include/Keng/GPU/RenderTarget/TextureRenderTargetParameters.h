#pragma once

#include "Keng/GPU/Resource/ITexture.h"

namespace keng::gpu
{
    class TextureRenderTargetParameters
    {
    public:
        ITexturePtr renderTarget;
    };
}
