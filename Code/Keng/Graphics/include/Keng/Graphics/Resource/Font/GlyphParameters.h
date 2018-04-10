#pragma once

#include <cstdint>
#include "Keng/Graphics/FwdDecl.h"

namespace keng::graphics
{
    class GlyphParameters
    {
    public:
        size_t size = 14;
        size_t dpiX = 96;
        size_t dpiY = 96;
    };
}
