#pragma once

#include "EverydayTools/EnumFlag.h"

namespace keng::graphics
{
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
}
