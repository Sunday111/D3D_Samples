#pragma once

#include "Keng/Graphics/RenderTarget/IRenderTarget.h"

namespace keng::graphics
{
    class IWindowRenderTarget : public IRenderTarget
    {
    public:
        virtual void Present() = 0;
        virtual void CopyFrom(const core::Ptr<ITexture>&) = 0;
        virtual ~IWindowRenderTarget() = default;
    };
}
