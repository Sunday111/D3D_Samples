#pragma once

#include "Keng/Core/FwdDecl.h"

namespace keng::reflection
{
	class IReflectionSystem;
	using IReflectionSystemPtr = core::Ptr<IReflectionSystem>;

	class IScope;
	class TypeOperators;
	class TypeInfo;
}