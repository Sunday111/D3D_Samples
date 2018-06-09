#pragma once

#include "Keng/GraphicsCommon/CpuAccessFlags.h"
#include "Keng/GraphicsCommon/DeviceBufferBindFlags.h"
#include "Keng/GraphicsCommon/DeviceBufferUsage.h"

namespace keng::graphics
{
    class DeviceBufferParameters
    {
    public:
        size_t size = 0;
        DeviceBufferUsage usage = DeviceBufferUsage::Default;
        DeviceBufferBindFlags bindFlags = DeviceBufferBindFlags::None;
        CpuAccessFlags accessFlags = CpuAccessFlags::None;
    };
}