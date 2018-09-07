#pragma once

#include "Keng/Reflection/CommonTypeInfo.h"

namespace keng::reflection
{
	template<typename T>
	class Reflector;

	template<typename T>
	class TypeInfo : public CommonTypeInfo
	{
	public:
		friend Reflector<T>;
	};
}
