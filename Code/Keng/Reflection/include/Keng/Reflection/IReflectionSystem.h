#pragma once

#include "FwdDecl.h"
#include "Keng/Core/ISystem.h"

namespace keng::reflection
{
    class IReflectionSystem : public core::ISystem
    {
    public:
		using RegistrationTask = void(*)(IScope& globalScope);

        static const char* SystemName() { return "KengReflection"; }

		virtual void AddRegistrationTask(RegistrationTask task) = 0;
    };
}
