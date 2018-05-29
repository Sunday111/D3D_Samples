#pragma once

#include "Keng/GraphicsAPI/FwdDecl.h"
#include "Keng/GraphicsAPI/RenderTarget/SwapChainParameters.h"

namespace keng::graphics_api
{
    class WindowRenderTargetParameters
    {
    public:
        SwapChainParameters swapChain;
    };
}