#pragma once

#include "Keng/GraphicsAPI/FwdDecl.h"

namespace keng::graphics_api
{
    class IRenderTarget : public core::IRefCountObject
    {
    public:
        virtual void AssignToPipeline(const IDepthStencilPtr& depthStencil) = 0;
        virtual void Clear(const float(&flatColor)[4]) = 0;
        virtual ~IRenderTarget() = default;
    };
}
