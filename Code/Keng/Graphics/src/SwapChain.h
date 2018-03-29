#pragma once

#include "d3d11.h"
#include "D3D_Tools/Texture.h"
#include "Keng/Graphics/ISwapChain.h"
#include "Keng/Graphics/Resource/TextureView.h"

namespace keng::graphics
{
    class Device;
    class ITexture;
    class Texture;

    class SwapChain :
        public core::RefCountImpl<ISwapChain>
    {
    public:
        SwapChain(Device& device, const SwapChainParameters& params);
        ~SwapChain();

        virtual core::Ptr<ITextureView> GetBackBufferView() override;
        virtual void Present() override;
        core::Ptr<ITexture> GetBackBuffer(uint32_t index = 0);
        ComPtr<ID3D11Texture2D> GetTexture(uint32_t index);
        void CopyFromTexture(const core::Ptr<Texture>& texture, uint32_t index);

    private:
        core::Ptr<Device> m_device;
        ComPtr<IDXGISwapChain> m_swapchain;
    };
}