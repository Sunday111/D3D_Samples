#pragma once

#include "Keng/Core/IRefCountObject.h"

namespace keng::graphics_api
{
    class IDevice : public core::IRefCountObject
    {
    public:
        virtual void* GetNativeDevice() const = 0;
        virtual void* GetNativeContext() const = 0;
        virtual ~IDevice() = default;
    };
}
