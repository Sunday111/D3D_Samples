#pragma once

#include "Keng/ResourceSystem/IDevice.h"
#include "Keng/GraphicsAPI/IDevice.h"

namespace keng::graphics
{
    class IDevice : public resource::IDevice
    {
    public:
        virtual graphics_api::IDevicePtr GetApiDevice() = 0;
        virtual ~IDevice() = default;
    };
}
