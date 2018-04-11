#pragma once

#include "Keng/Core/FwdDecl.h"

namespace keng::core
{
    class IGlobalEnvironment
    {
    public:
        static KENG_CORE_API IGlobalEnvironment& Instance();

        virtual IGlobalSystem& GetGlobalSystem(size_t id) = 0;
        virtual ~IGlobalEnvironment() = default;

        template<typename T>
        static inline T& GetGlobalSystem() {
            return static_cast<T&>(Instance().GetGlobalSystem(T::GetGlobalSystemId()));
        }
    };
}
