#pragma once

#include "EverydayTools\Geom\Vector.h"

namespace keng::gpu
{
    class DeviceParameters
    {
    public:
        bool debugDevice = false;
        bool noDeviceMultithreading = false;
    };
}