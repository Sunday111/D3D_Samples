#include "Device.h"
#include "SwapChain.h"
#include "Keng/WindowSystem/IWindow.h"
#include "Resource/Texture/DeviceTexture.h"
#include "DepthStencil.h"
#include "Keng/GraphicsCommon/WindowRenderTargetParameters.h"
#include "RenderTarget/WindowRenderTarget.h"
#include "EverydayTools/Exception/ThrowIfFailed.h"

namespace keng::graphics::gpu
{
    WindowRenderTarget::WindowRenderTarget(Device& device, const WindowRenderTargetParameters& params, window_system::IWindow& window)
    {
        m_device = &device;
        m_swapChain = SwapChainPtr::MakeInstance(device, params.swapChain, window);

        D3D11_BLEND_DESC desc{};
        desc.RenderTarget[0].BlendEnable = true;
        desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
        desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
        desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
        desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
        desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
        desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
        WinAPI<char>::ThrowIfError(m_device->GetDevice()->CreateBlendState(&desc, m_blendState.Receive()));
    }

    void WindowRenderTarget::AssignToPipeline(const IDepthStencilPtr& depthStencil) {
        auto rtv = m_swapChain->GetCurrentTexture().GetView<ResourceViewType::RenderTarget>();
        core::Ptr<TextureView<ResourceViewType::DepthStencil>> dsv;
        if (depthStencil) {
            auto castedDepthStencil = std::dynamic_pointer_cast<DepthStencil>(depthStencil);
            edt::ThrowIfFailed(castedDepthStencil, "Custom depth stencil implementation is not supported");
            dsv = castedDepthStencil->GetView();
        }
        m_device->SetRenderTarget(*rtv, dsv.Get());

        m_device->GetContext()->OMSetBlendState(m_blendState.Get(), 0, 0xFFFFFFFF);
    }

    void WindowRenderTarget::Clear(const float(&flatColor)[4]) {
        auto rtv = m_swapChain->GetCurrentTexture().GetView<ResourceViewType::RenderTarget>();
        m_device->GetContext()->ClearRenderTargetView(rtv->GetView(), flatColor);
    }

    void WindowRenderTarget::CopyFrom(const ITexture& abstract) {
        CallAndRethrowM + [&] {
            auto from = static_cast<const DeviceTexture&>(abstract);
            m_swapChain->CopyFromTexture(from);
        };
    }

    void WindowRenderTarget::Present() {
        m_swapChain->Present();
    }
}