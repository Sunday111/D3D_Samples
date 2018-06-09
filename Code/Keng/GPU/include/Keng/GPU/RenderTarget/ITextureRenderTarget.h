#pragma once

#include "Keng/GPU/RenderTarget/IRenderTarget.h"

namespace keng::graphics::gpu
{
    class ITextureRenderTarget : public IRenderTarget
    {
    public:
        virtual ITexture& GetTexture() const = 0;
    };
}