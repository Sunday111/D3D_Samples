#include "Keng/Graphics/Device.h"
#include "SwapChain.h"
#include "Keng/WindowSystem/IWindow.h"
#include "Resource/Texture/Texture.h"
#include "DepthStencil.h"
#include "Keng/Graphics/RenderTarget/WindowRenderTargetParameters.h"
#include "RenderTarget/WindowRenderTarget.h"

namespace keng::graphics
{
    WindowRenderTarget::WindowRenderTarget(Device& device, const WindowRenderTargetParameters& params)
    {
        m_device = &device;
        m_swapChain = core::Ptr<SwapChain>::MakeInstance(device, params.swapChain);
    }

    void WindowRenderTarget::AssignToPipeline(const IDepthStencilPtr& depthStencil) {
        auto rtv = m_swapChain->GetCurrentTexture()->GetView<ResourceViewType::RenderTarget>();
        core::Ptr<TextureView<ResourceViewType::DepthStencil>> dsv;
        if (depthStencil) {
            auto castedDepthStencil = std::dynamic_pointer_cast<DepthStencil>(depthStencil);
            edt::ThrowIfFailed(castedDepthStencil, "Custom depth stencil implementation is not supported");
            dsv = castedDepthStencil->GetView();
        }
        m_device->SetRenderTarget(*rtv, dsv.Get());
    }

    void WindowRenderTarget::Clear(const float(&flatColor)[4]) {
        auto rtv = m_swapChain->GetCurrentTexture()->GetView<ResourceViewType::RenderTarget>();
        m_device->GetContext()->ClearRenderTargetView(rtv->GetView(), flatColor);
    }

    void WindowRenderTarget::CopyFrom(const ITexturePtr& abstract) {
        CallAndRethrowM + [&] {
            auto from = std::static_pointer_cast<Texture>(abstract);
            m_swapChain->CopyFromTexture(from);
        };
    }

    void WindowRenderTarget::Present() {
        m_swapChain->Present();
    }
}