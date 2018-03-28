#pragma once

#include "EverydayTools/Array/ArrayView.h"
#include "Keng/Core/IRefCountObject.h"
#include "Keng/Core/Ptr.h"

namespace keng::graphics
{
    class IDepthStencil;
    class ITextureView;

    class IRenderTarget : public core::IRefCountObject
    {
    public:
        virtual void Activate(const core::Ptr<IDepthStencil>& depthStencil) = 0;
        virtual void ClearRenderTarget(const float(&flatColor)[4]) = 0;
        virtual core::Ptr<ITextureView> GetRenderTargetView() = 0;
        virtual ~IRenderTarget() = default;
    };
}
