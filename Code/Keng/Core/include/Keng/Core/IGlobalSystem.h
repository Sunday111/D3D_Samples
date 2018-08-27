#pragma once

#include "Keng/Core/ISystem.h"

namespace keng::core
{
    class IGlobalSystem : public ISystem
    {
    public:
        virtual ~IGlobalSystem() = default;
    };
}
