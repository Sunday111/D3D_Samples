#pragma once

#include "Keng/Graphics/Resource/ITexture.h"

namespace keng::graphics
{
    class DepthStencilParameters
    {
    public:
        FragmentFormat format;
        core::Ptr<ITexture> texture;
    };
}
