#pragma once

#include "d3d11.h"
#include "D3D_Tools/Texture.h"
#include "Keng/Graphics/ISwapChain.h"
#include "Keng/Graphics/Device.h"

namespace keng::graphics
{
    class SwapChain : public ISwapChain
    {
    public:
        SwapChain(ID3D11Device* device, uint32_t w, uint32_t h, HWND hWnd, DXGI_FORMAT format);
        ~SwapChain();

        virtual core::Ptr<ITextureView> GetBackBufferView(IDevice* device) override;
        virtual void Present() override;
        d3d_tools::Texture GetBackBuffer(uint32_t index = 0);

    private:
        ComPtr<IDXGISwapChain> m_swapchain;
        core::Ptr<TextureView<ResourceViewType::RenderTarget>> m_renderTargetView;
    };
}