#pragma once

#include "Keng/GraphicsAPI/FilteringMode.h"
#include "Keng/GraphicsAPI/TextureAddressMode.h"

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
