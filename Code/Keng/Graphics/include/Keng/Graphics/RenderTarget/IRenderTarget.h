#pragma once

#include "Keng/Core/IRefCountObject.h"
#include "Keng/Graphics/Resource/Texture.h"
#include "EverydayTools/Array/ArrayView.h"

namespace keng::graphics
{
    class IRenderTarget : public core::IRefCountObject
    {
    public:
        virtual core::Ptr<ITextureView> GetRenderTargetView() = 0;
        virtual core::Ptr<ITextureView> GetDepthStencilView() = 0;
        virtual ~IRenderTarget() = default;
    };
}
