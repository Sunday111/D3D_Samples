#pragma once

#include "D3D_Tools/SwapChain.h"

namespace keng::graphics
{
    class ISwapChain
    {
    public:
        virtual void* GetNativeInterface() const = 0;
        virtual ~ISwapChain() = default;
    };

    class SwapChain :
        public ISwapChain,
        public d3d_tools::SwapChain
    {
    public:
        using d3d_tools::SwapChain::SwapChain;

        void* GetNativeInterface() const {
            return GetInterface();
        }
    };
}