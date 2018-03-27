#pragma once

#include "Keng/Graphics/Resource/Texture.h"

namespace keng::graphics
{
    class DepthStencilParameters
    {
    public:
        d3d_tools::TextureFormat format;
        core::Ptr<ITexture> texture;
    };
}
