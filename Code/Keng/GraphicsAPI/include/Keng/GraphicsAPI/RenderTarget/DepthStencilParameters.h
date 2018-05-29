#pragma once

#include "Keng/GraphicsAPI/FragmentFormat.h"
#include "Keng/GraphicsAPI/Resource/ITexture.h"

namespace keng::graphics_api
{
    class DepthStencilParameters
    {
    public:
        FragmentFormat format;
        ITexturePtr texture;
    };
}
