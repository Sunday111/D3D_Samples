#pragma once

#include "EverydayTools/Bitset/EnumBitset.h"

namespace keng::graphics
{
    enum class CpuAccessFlags
    {
        None = 0,
        Read = (1 << 0),
        Write = (1 << 1),
        Last = (1 << 2)
    };
}

namespace edt
{
	template<>
	struct enable_enum_bitset<keng::graphics::CpuAccessFlags>
	{
		static constexpr bool value = true;
	};
}
