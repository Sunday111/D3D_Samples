#pragma once

#include "EverydayTools/EnumFlag.h"
#include "Keng/GraphicsCommon/CpuAccessFlags.h"

namespace keng::graphics
{
    enum class DeviceBufferUsage
    {
        Default,
        Immutable,
        Dynamic,
        Staging,
        Last
    };

    enum class DeviceBufferBindFlags
    {
        None = 0,
        VertexBuffer = (1 << 0),
        IndexBuffer = (1 << 1),
        ConstantBuffer = (1 << 2),
        ShaderResource = (1 << 3),
        RenderTarget = (1 << 5),
        DepthStencil = (1 << 6),
        UnorderedAccess = (1 << 7),
        Last
    };

    EDT_ENUM_FLAG_OPERATORS(DeviceBufferBindFlags);

    class DeviceBufferParameters
    {
    public:
        size_t size = 0;
        DeviceBufferUsage usage = DeviceBufferUsage::Default;
        DeviceBufferBindFlags bindFlags = DeviceBufferBindFlags::None;
        CpuAccessFlags accessFlags = CpuAccessFlags::None;
    };
}