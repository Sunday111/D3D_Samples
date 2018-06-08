#pragma once

#include "EverydayTools/EnumFlag.h"

namespace keng::gpu
{
    enum class TextureUsage
    {
        None = 0,
        RenderTarget = (1 << 0),
        DepthStencil = (1 << 1),
        ShaderResource = (1 << 2)
    };

    EDT_ENUM_FLAG_OPERATORS(TextureUsage);
}
