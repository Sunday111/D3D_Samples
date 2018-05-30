#pragma once

#include "Keng/ResourceSystem/IDevice.h"
#include "Keng/GPU/IDevice.h"

namespace keng::graphics
{
    class IDevice : public resource::IDevice
    {
    public:
        virtual gpu::IDevicePtr GetApiDevice() = 0;
        virtual ~IDevice() = default;
    };
}
