#pragma once

#include "Keng/GPU/FwdDecl.h"
#include "Keng/GPU/RenderTarget/SwapChainParameters.h"

namespace keng::graphics::gpu
{
    class WindowRenderTargetParameters
    {
    public:
        SwapChainParameters swapChain;
    };
}