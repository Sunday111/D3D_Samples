#pragma once

#include "Keng/Graphics/FwdDecl.h"

namespace keng::graphics
{
    class ISwapChain : public core::IRefCountObject
    {
    public:
        virtual void Present() = 0;
        virtual ~ISwapChain() = default;
    };
}
