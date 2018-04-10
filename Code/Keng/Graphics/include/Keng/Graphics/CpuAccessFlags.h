#pragma once

#include "EverydayTools/EnumFlag.h"

namespace keng::graphics
{
    enum class CpuAccessFlags
    {
        None = 0,
        Read = (1 << 1),
        Write = (1 << 2)
    };

    EDT_ENUM_FLAG_OPERATORS(CpuAccessFlags);
}
