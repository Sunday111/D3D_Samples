#pragma once

#include "EverydayTools\Geom\Vector.h"

namespace keng::graphics_api
{
    class DeviceParameters
    {
    public:
        bool debugDevice = false;
        bool noDeviceMultithreading = false;
    };
}