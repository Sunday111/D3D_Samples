#pragma once

#include "Keng/GPU/RenderTarget/IRenderTarget.h"

namespace keng::gpu
{
    class IWindowRenderTarget : public IRenderTarget
    {
    public:
        virtual void Present() = 0;
        virtual void CopyFrom(const ITexturePtr&) = 0;
        virtual ~IWindowRenderTarget() = default;
    };
}
