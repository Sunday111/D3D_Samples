#pragma once

#include "Keng/GraphicsAPI/RenderTarget/IRenderTarget.h"

namespace keng::graphics_api
{

    class ITextureRenderTarget : public IRenderTarget
    {
    public:
        virtual ITexturePtr GetTexture() const = 0;
    };
}