#include "Keng/Reflection/TypeIsReflected.h"

namespace keng::reflection
{
	class T0
	{
	public:
		static void ReflectType(Reflector<T0>) {}
	};

	class T1
	{
	};

	void ReflectType(Reflector<T1>) {}


	class T2
	{
	};

	static_assert(type_is_reflected<T0>);
	static_assert(type_is_reflected<T1>);
	static_assert(!type_is_reflected<T2>);
}