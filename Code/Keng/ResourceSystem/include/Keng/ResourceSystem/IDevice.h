#pragma once

#include "Keng/Core/IRefCountObject.h"

namespace keng::resource
{
    class IDevice : public core::IRefCountObject
    {
    public:
        virtual ~IDevice() = default;
    };
}