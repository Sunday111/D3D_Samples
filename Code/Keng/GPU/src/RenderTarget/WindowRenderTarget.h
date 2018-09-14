#pragma once

#include "FwdDecl.h"
#include "Keng/GPU/RenderTarget/IWindowRenderTarget.h"

namespace keng::graphics::gpu
{
    class WindowRenderTarget :
        public core::RefCountImpl<IWindowRenderTarget>
    {
    public:
        WindowRenderTarget(Device& device, const WindowRenderTargetParameters& params, window_system::IWindow& window);
        virtual void AssignToPipeline(const IDepthStencilPtr& depthStencil) override final;
        virtual void Clear(const float(&flatColor)[4]) override final;
        virtual void CopyFrom(const ITexture&) override final;
        virtual void Present() override final;

    private:
        DevicePtr m_device;
        SwapChainPtr m_swapChain;

        ComPtr<ID3D11BlendState> m_blendState;
    };
}
