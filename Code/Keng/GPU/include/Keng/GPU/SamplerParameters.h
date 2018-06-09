#pragma once

#include "Keng/GPU/FilteringMode.h"
#include "Keng/GraphicsCommon/TextureAddressMode.h"

namespace keng::graphics::gpu
{
    class SamplerParameters
    {
    public:
        TextureAddressMode addressU = TextureAddressMode::Clamp;
        TextureAddressMode addressV = TextureAddressMode::Clamp;
        TextureAddressMode addressW = TextureAddressMode::Clamp;
        FilteringMode filter = FilteringMode::Nearest;
        float borderColor[4]{1.f, 1.f, 1.f, 1.f};
    };
}
