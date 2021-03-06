#pragma once

#include "Keng/GPU/FwdDecl.h"

namespace keng::graphics::gpu
{
    class ISwapChain : public core::IRefCountObject
    {
    public:
        virtual void Present() = 0;
        virtual ~ISwapChain() = default;
    };
}
