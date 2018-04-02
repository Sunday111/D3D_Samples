#pragma once

#include "Keng/Graphics/TextureUsage.h"
#include "Keng/Graphics/FragmentFormat.h"

namespace keng::graphics
{
    class TextureParameters
    {
    public:
        uint32_t width = 0;
        uint32_t height = 0;
        TextureUsage usage = TextureUsage::ShaderResource;
        FragmentFormat format = FragmentFormat::R8_G8_B8_A8_UNORM;
        const void* data = nullptr;
    };
}