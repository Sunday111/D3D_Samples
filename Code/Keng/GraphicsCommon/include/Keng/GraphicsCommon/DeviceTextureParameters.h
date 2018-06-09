#pragma once

#include "Keng/GraphicsCommon/TextureUsage.h"
#include "Keng/GraphicsCommon/FragmentFormat.h"
#include "Keng/GraphicsCommon/CpuAccessFlags.h"

namespace keng::graphics
{
    class DeviceTextureParameters
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