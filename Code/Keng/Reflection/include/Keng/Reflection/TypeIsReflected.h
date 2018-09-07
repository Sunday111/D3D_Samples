#pragma once

#include "EverydayTools/Template/TypeHasMethod.h"
#include "Keng/Reflection/Reflector.h"

namespace keng::reflection::detail
{
	ImplementHasMethod(ReflectType);

	template<typename T>
	constexpr bool HasReflectTypeOverload(...) {
		return false;
	}

	template<typename T>
	constexpr auto HasReflectTypeOverload(int)
		-> decltype(ReflectType(std::declval<Reflector<T>>()), bool())
	{
		return true;
	}
}

namespace keng::reflection
{
	template<typename T>
	constexpr inline bool type_is_reflected = detail::HasReflectTypeMethod_v<T> || detail::HasReflectTypeOverload<T>(0);
}
