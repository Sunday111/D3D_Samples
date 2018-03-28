#pragma once

#include "Keng/Graphics/Resource/ITexture.h"

namespace keng::graphics
{
    class DepthStencilParameters
    {
    public:
        d3d_tools::TextureFormat format;
        core::Ptr<ITexture> texture;
    };
}
