#pragma once

#include "Keng/GPU/FwdDecl.h"

namespace keng::graphics::gpu
{
    class IRenderTarget : public core::IRefCountObject
    {
    public:
        virtual void AssignToPipeline(const IDepthStencilPtr& depthStencil) = 0;
        virtual void Clear(const float(&flatColor)[4]) = 0;
        virtual ~IRenderTarget() = default;
    };
}
