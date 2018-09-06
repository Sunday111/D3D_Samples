#pragma once

#include "FwdDecl.h"
#include "Keng/Core/ISystem.h"

namespace keng::reflection
{
    class IReflectionSystem : public core::ISystem
    {
    public:
        static const char* SystemName() { return "KengReflection"; }
    };
}
