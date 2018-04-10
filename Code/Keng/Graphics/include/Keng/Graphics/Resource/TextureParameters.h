#pragma once

#include "Keng/Graphics/TextureUsage.h"
#include "Keng/Graphics/FragmentFormat.h"
#include "Keng/Graphics/CpuAccessFlags.h"

namespace keng::graphics
{
    class TextureParameters
    {
    public:
        size_t width = 0;
        size_t height = 0;
        TextureUsage usage = TextureUsage::ShaderResource;
        FragmentFormat format = FragmentFormat::R8_G8_B8_A8_UNORM;
        CpuAccessFlags cpuAccessFlags = CpuAccessFlags::None;
        const void* data = nullptr;
    };
}