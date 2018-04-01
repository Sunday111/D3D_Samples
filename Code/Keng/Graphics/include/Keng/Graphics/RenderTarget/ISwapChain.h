#pragma once

#include "Keng/Core/Ptr.h"
#include "Keng/Core/IRefCountObject.h"
#include "Keng/Graphics/FragmentFormat.h"

namespace keng::graphics
{
    class ISwapChain : public core::IRefCountObject
    {
    public:
        virtual void Present() = 0;
        virtual ~ISwapChain() = default;
    };
}
