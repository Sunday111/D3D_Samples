#pragma once

#include "EverydayTools/Bitset/EnumBitset.h"

namespace keng::graphics
{
    enum class DepthStencilClearFlags
    {
        None = 0,
        ClearDepth = (1 << 0),
        ClearStencil = (1 << 1),
        Last = (1<<2)
    };
}

namespace edt
{
	template<>
	struct enable_enum_bitset<keng::graphics::DepthStencilClearFlags>
	{
		static constexpr bool value = true;
	};
}