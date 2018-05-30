#pragma once

#include "Keng/GPU/FwdDecl.h"
#include "Keng/GPU/FragmentFormat.h"
#include <cstdint>

namespace keng::gpu
{
    class SwapChainParameters
    {
    public:
        size_t width = 0;
        size_t height = 0;
        size_t buffers = 1;
        FragmentFormat format = FragmentFormat::R8_G8_B8_A8_UNORM;
        window_system::IWindow* window;
    };
}