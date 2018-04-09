#pragma once

#include <cstdint>
#include "Keng/Graphics/FwdDecl.h"

namespace keng::graphics
{
    class GlyphInfo
    {
    public:
        size_t unicode;

        size_t horizontalBearingX;
        size_t horizontalBearingY;

        size_t verticalBearingX;
        size_t verticalBearingY;

        size_t width;
        size_t height;

        size_t advanceX;
        size_t advanceY;
    };

    class AtlasGlyphInfo : public GlyphInfo
    {
    public:
        size_t x;
        size_t y;
        ITexturePtr texture;
    };
}
