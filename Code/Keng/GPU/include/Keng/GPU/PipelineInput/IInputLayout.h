#pragma once

#include "Keng/Core/IRefCountObject.h"

namespace keng::graphics::gpu
{
    class IInputLayout : public core::IRefCountObject
    {
    public:
        virtual void AssignToPipeline() = 0;
    };
}
