#pragma once

#include "Keng/Graphics/FilteringMode.h"
#include "Keng/Graphics/TextureAddressMode.h"

namespace keng::graphics_api
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
