#pragma once

#include "Keng/Core/ISystem.h"

namespace keng::core
{
    class IGlobalSystem : public ISystem
    {
    public:
        virtual void AssignId(size_t id) = 0;
        virtual ~IGlobalSystem() = default;
    };
}
