#pragma once

#include "Keng/Graphics/FwdDecl.h"
#include "Keng/Graphics/RenderTarget/SwapChainParameters.h"

namespace keng::graphics_api
{
    class WindowRenderTargetParameters
    {
    public:
        SwapChainParameters swapChain;
    };
}