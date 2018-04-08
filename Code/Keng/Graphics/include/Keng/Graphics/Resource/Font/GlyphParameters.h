#pragma once

#include <cstdint>
#include "Keng/Graphics/FwdDecl.h"

namespace keng::graphics
{
    class GlyphParameters
    {
    public:
        uint32_t size = 14;
        uint32_t dpiX = 96;
        uint32_t dpiY = 96;
    };
}
