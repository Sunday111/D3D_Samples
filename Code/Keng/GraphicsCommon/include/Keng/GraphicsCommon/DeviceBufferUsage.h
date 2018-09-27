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
}

namespace edt
{
	template<>
	struct enable_enum_flags<keng::graphics::DeviceBufferUsage>
	{
		static constexpr bool value = true;
	};
}
