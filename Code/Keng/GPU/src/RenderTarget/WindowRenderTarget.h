#pragma once

#include "FwdDecl.h"
#include "Keng/GPU/RenderTarget/IWindowRenderTarget.h"

namespace keng::graphics::gpu
{
    class WindowRenderTarget :
        public core::RefCountImpl<IWindowRenderTarget>
    {
    public:
        WindowRenderTarget(Device& device, const WindowRenderTargetParameters& params);
        virtual void AssignToPipeline(const IDepthStencilPtr& depthStencil) override;
        virtual void Clear(const float(&flatColor)[4]) override;
        virtual void CopyFrom(const ITexturePtr&) override;
        virtual void Present() override;

    private:
        DevicePtr m_device;
        SwapChainPtr m_swapChain;

        ComPtr<ID3D11BlendState> m_blendState;
    };
}
