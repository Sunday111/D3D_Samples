#pragma once

#include "d3d11.h"
#include "D3D_Tools/Texture.h"
#include "Keng/Graphics/ISwapChain.h"

namespace keng::graphics
{
    class Device;

    class SwapChain : public ISwapChain
    {
        IMPLEMENT_IREFCOUNT
    public:
        SwapChain(Device& device, const SwapChainParameters& params);
        ~SwapChain();

        virtual core::Ptr<ITextureView> GetBackBufferView() override;
        virtual void Present() override;
        d3d_tools::Texture GetBackBuffer(uint32_t index = 0);

    private:
        core::Ptr<Device> m_device;
        ComPtr<IDXGISwapChain> m_swapchain;
        core::Ptr<TextureView<ResourceViewType::RenderTarget>> m_renderTargetView;
    };
}