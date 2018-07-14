#pragma once

#include "EverydayTools/EnumFlag.h"

namespace keng::graphics
{
    enum class DepthStencilClearFlags
    {
        None = 0,
        ClearDepth = (1 << 0),
        ClearStencil = (1 << 1),
        Last = (1<<2)
    };

    EDT_ENUM_FLAG_OPERATORS(DepthStencilClearFlags);
}
