#pragma once

#include "EverydayTools/EnumFlag.h"

namespace keng::graphics
{
    enum class DeviceBufferUsage
    {
        Default,
        Immutable,
        Dynamic,
        Staging,
        Last
    };

    EDT_ENUM_FLAG_OPERATORS(DeviceBufferUsage);
}
