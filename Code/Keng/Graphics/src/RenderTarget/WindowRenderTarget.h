#pragma once

#include "Keng/Graphics/RenderTarget/IWindowRenderTarget.h"

namespace keng::graphics
{
    class Device;
    class WindowRenderTargetParameters;
    class SwapChain;

    class WindowRenderTarget :
        public core::RefCountImpl<IWindowRenderTarget>
    {
    public:
        WindowRenderTarget(Device& device, const WindowRenderTargetParameters& params);
        virtual void AssignToPipeline(const core::Ptr<IDepthStencil>& depthStencil) override;
        virtual void Clear(const float(&flatColor)[4]) override;
        virtual void CopyFrom(const core::Ptr<ITexture>&) override;
        virtual void Present() override;

    private:
        core::Ptr<Device> m_device;
        core::Ptr<SwapChain> m_swapChain;
    };
}
