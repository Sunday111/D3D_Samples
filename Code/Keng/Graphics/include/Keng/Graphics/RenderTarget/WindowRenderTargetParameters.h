#pragma once

#include "Keng/Graphics/FwdDecl.h"
#include "Keng/Graphics/RenderTarget/SwapChainParameters.h"

namespace keng::window_system
{
    class IWindow;
}

namespace keng::graphics
{
    class WindowRenderTargetParameters
    {
    public:
        SwapChainParameters swapChain;
    };
}