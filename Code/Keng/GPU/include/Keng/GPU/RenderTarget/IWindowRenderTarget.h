#pragma once

#include "Keng/GPU/RenderTarget/IRenderTarget.h"

namespace keng::graphics::gpu
{
    class IWindowRenderTarget : public IRenderTarget
    {
    public:
        virtual void Present() = 0;
        virtual void CopyFrom(const ITexture&) = 0;
        virtual ~IWindowRenderTarget() = default;
    };
}
