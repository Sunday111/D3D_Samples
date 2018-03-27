#include "Keng/Graphics/Device.h"
#include "SwapChain.h"
#include "Keng/WindowSystem/IWindow.h"
#include "D3D_Tools/Texture.h"
#include "DepthStencil.h"
#include "Keng/Graphics/RenderTarget/WindowRenderTargetParameters.h"
#include "RenderTarget/WindowRenderTarget.h"

namespace keng::graphics
{
    WindowRenderTarget::WindowRenderTarget(Device& device, const WindowRenderTargetParameters& params)
    {
        m_device = &device;
        uint32_t w, h;
        params.window->GetClientSize(&w, &h);
        SwapChainParameters scp{};
        scp.width = w;
        scp.height = h;
        scp.window = params.window;
        scp.format = params.format;
        m_swapChain = core::Ptr<SwapChain>::MakeInstance(device, scp);
    }

    void WindowRenderTarget::Activate(const core::Ptr<IDepthStencil>& depthStencil) {
        auto rtv = std::dynamic_pointer_cast<TextureView<ResourceViewType::RenderTarget>>(GetRenderTargetView());
        assert(rtv);
        core::Ptr<TextureView<ResourceViewType::DepthStencil>> dsv;
        if (depthStencil) {
            auto castedDepthStencil = std::dynamic_pointer_cast<DepthStencil>(depthStencil);
            edt::ThrowIfFailed(castedDepthStencil, "Custom depth stencil implementation is not supported");
            dsv = castedDepthStencil->GetView();
        }
        m_device->SetRenderTarget(*rtv, dsv.Get());
    }

    void WindowRenderTarget::ClearRenderTarget(const float(&flatColor)[4]) {
        auto rtv = std::dynamic_pointer_cast<TextureView<ResourceViewType::RenderTarget>>(GetRenderTargetView());
        assert(rtv);
        m_device->GetContext()->ClearRenderTargetView(rtv->GetView(), flatColor);
    }

    core::Ptr<ITextureView> WindowRenderTarget::GetRenderTargetView() {
        return m_swapChain->GetBackBufferView();
    }

    void WindowRenderTarget::Present() {
        m_swapChain->Present();
    }
}