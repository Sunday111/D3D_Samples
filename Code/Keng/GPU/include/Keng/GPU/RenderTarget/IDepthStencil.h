#pragma once

#include "Keng/GPU/FwdDecl.h"
#include "EverydayTools/EnumFlag.h"
#include <cstdint>

namespace keng::graphics::gpu
{
    enum class DepthStencilClearFlags
    {
        None = 0,
        ClearDepth = (1 << 0),
        ClearStencil = (1 << 1)
    };

    EDT_ENUM_FLAG_OPERATORS(DepthStencilClearFlags);

    class IDepthStencil : public core::IRefCountObject
    {
    public:
        virtual void Clear(DepthStencilClearFlags flags, float depth, uint8_t stencil) = 0;
        virtual ~IDepthStencil() = default;
    };
}
