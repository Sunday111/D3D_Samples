#pragma once

#include "EverydayTools/Exception/CallAndRethrow.h"
#include "Keng/Reflection/Detail/MakeTypeOperators.h"
#include "Keng/Reflection/TypeInfo.h"

namespace keng::reflection::detail
{
	template<typename T>
	TypeInfo<T> MakeTypeInfo() {
		return CallAndRethrowM + [&] {
			TypeInfo ti;
			ti.operators = MakeTypeOperators<T>();
			return ti;
		};
	}
}
