#pragma once

#include "EverydayTools/GUID.h"
#include "Keng/Reflection/TypeOperators.h"

namespace keng::reflection
{
	class CommonTypeInfo
	{
	public:
		TypeOperators operators;
		edt::GUID guid;
	};
}
