#pragma once

#include "Keng/GraphicsCommon/FwdDecl.h"
#include "Keng/GraphicsCommon/FragmentFormat.h"
#include <cstdint>

namespace keng::graphics
{
    class SwapChainParameters
    {
    public:
        size_t width = 0;
        size_t height = 0;
        size_t buffers = 1;
        FragmentFormat format = FragmentFormat::R8_G8_B8_A8_UNORM;
    };
}