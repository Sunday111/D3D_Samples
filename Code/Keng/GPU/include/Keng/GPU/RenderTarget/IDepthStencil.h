#pragma once

#include "Keng/GPU/FwdDecl.h"
#include "EverydayTools/Bitset/EnumBitset.h"
#include <cstdint>

namespace keng::graphics::gpu
{

    class IDepthStencil : public core::IRefCountObject
    {
    public:
        virtual void Clear(const DepthStencilClearFlags& flags, float depth, uint8_t stencil) = 0;
        virtual ~IDepthStencil() = default;
    };
}
