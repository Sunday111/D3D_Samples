#pragma once

#include "Keng/ResourceSystem/IDevice.h"

namespace keng::graphics
{
    class IDevice : public resource::IDevice
    {
    public:
        virtual void* GetNativeDevice() const = 0;
        virtual void* GetNativeContext() const = 0;
        virtual ~IDevice() = default;
    };
}
