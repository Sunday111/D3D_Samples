#pragma once

#include "Keng/GPU/RenderTarget/IRenderTarget.h"

namespace keng::gpu
{

    class ITextureRenderTarget : public IRenderTarget
    {
    public:
        virtual ITexturePtr GetTexture() const = 0;
    };
}