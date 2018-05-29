#pragma once

#include "Keng/GraphicsAPI/TextureUsage.h"
#include "Keng/GraphicsAPI/FragmentFormat.h"
#include "Keng/GraphicsAPI/CpuAccessFlags.h"

namespace keng::graphics_api
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