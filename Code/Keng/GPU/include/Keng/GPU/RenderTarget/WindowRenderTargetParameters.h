#pragma once

#include "Keng/GPU/FwdDecl.h"
#include "Keng/GPU/RenderTarget/SwapChainParameters.h"

namespace keng::gpu
{
    class WindowRenderTargetParameters
    {
    public:
        SwapChainParameters swapChain;
    };
}