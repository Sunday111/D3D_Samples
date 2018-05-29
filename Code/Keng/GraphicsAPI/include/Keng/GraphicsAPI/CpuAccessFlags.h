#pragma once

#include "EverydayTools/EnumFlag.h"

namespace keng::graphics_api
{
    enum class CpuAccessFlags
    {
        None = 0,
        Read = (1 << 0),
        Write = (1 << 1)
    };

    EDT_ENUM_FLAG_OPERATORS(CpuAccessFlags);
}
