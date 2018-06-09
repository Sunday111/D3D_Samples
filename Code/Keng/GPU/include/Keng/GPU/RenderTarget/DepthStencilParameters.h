#pragma once

#include "Keng/GraphicsCommon/FragmentFormat.h"
#include "Keng/GPU/Resource/ITexture.h"

namespace keng::graphics::gpu
{
    class DepthStencilParameters
    {
    public:
        FragmentFormat format;
        ITexturePtr texture;
    };
}
