#pragma once

#include "Keng/GraphicsAPI/RenderTarget/IRenderTarget.h"

namespace keng::graphics_api
{
    class IWindowRenderTarget : public IRenderTarget
    {
    public:
        virtual void Present() = 0;
        virtual void CopyFrom(const ITexturePtr&) = 0;
        virtual ~IWindowRenderTarget() = default;
    };
}
