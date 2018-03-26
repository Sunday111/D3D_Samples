#pragma once

#include "Keng/Graphics/RenderTarget/IWindowRenderTarget.h"

namespace keng::graphics
{
    class Device;
    class WindowRenderTargetParameters;
    class SwapChain;

    class WindowRenderTarget : public IWindowRenderTarget
    {
        IMPLEMENT_IREFCOUNT
    public:
        WindowRenderTarget(Device& device, const WindowRenderTargetParameters& params);
        virtual core::Ptr<ITextureView> GetRenderTargetView() override;
        virtual core::Ptr<ITextureView> GetDepthStencilView() override;
        virtual void Present() override;

    private:
        core::Ptr<Device> m_device;
        std::shared_ptr<SwapChain> m_swapChain;
    };
}
