#include "RenderTarget/WindowRenderTarget.h"
#include "Keng/Graphics/Device.h"
#include "SwapChain.h"
#include "Keng/WindowSystem/IWindow.h"
#include "D3D_Tools/Texture.h"

namespace keng::graphics
{
    WindowRenderTarget::WindowRenderTarget(Device& device, const WindowRenderTargetParameters& params)
    {
        m_device = &device;
        uint32_t w, h;
        params.window->GetClientSize(&w, &h);
        m_swapChain = std::make_shared<SwapChain>(
            device.GetDevice(), w, h, (HWND)params.window->GetNativeHandle(),
            d3d_tools::texture_details::ConvertFormat(params.format));
    }

    core::Ptr<ITextureView> WindowRenderTarget::GetRenderTargetView() {
        return m_swapChain->GetBackBufferView(m_device.Get());
    }

    core::Ptr<ITextureView> WindowRenderTarget::GetDepthStencilView() {
        return nullptr;
    }

    void WindowRenderTarget::Present() {
        m_swapChain->Present();
    }
}