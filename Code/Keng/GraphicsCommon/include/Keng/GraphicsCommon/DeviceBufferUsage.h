#pragma once

#include "EverydayTools/Bitset/EnumBitset.h"

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
	struct enable_enum_bitset<keng::graphics::DeviceBufferUsage>
	{
		static constexpr bool value = true;
	};
}
