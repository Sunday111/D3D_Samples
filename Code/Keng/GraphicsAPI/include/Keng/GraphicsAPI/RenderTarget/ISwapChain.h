#pragma once

#include "Keng/GraphicsAPI/FwdDecl.h"

namespace keng::graphics_api
{
    class ISwapChain : public core::IRefCountObject
    {
    public:
        virtual void Present() = 0;
        virtual ~ISwapChain() = default;
    };
}
