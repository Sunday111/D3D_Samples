#pragma once

#include "Keng/GPU/Resource/ITexture.h"

namespace keng::graphics::gpu
{
    class TextureRenderTargetParameters
    {
    public:
        ITexturePtr renderTarget;
    };
}
