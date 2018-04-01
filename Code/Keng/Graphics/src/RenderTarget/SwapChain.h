#pragma once

#include "d3d11.h"
#include "D3D_Tools/Texture.h"
#include "Keng/Graphics/RenderTarget/ISwapChain.h"
#include "Keng/Graphics/Resource/TextureView.h"

namespace keng::graphics
{
    class Device;
    class ITexture;
    class Texture;
    class SwapChainParameters;

    class SwapChain :
        public core::RefCountImpl<ISwapChain>
    {
    public:
        SwapChain(Device& device, const SwapChainParameters& params);
        ~SwapChain();

        virtual void Present() override;
        core::Ptr<Texture> GetCurrentTexture();
        void CopyFromTexture(const core::Ptr<Texture>& texture);

    private:
        ComPtr<ID3D11Texture2D> GetBackBuffer(uint32_t index);

        core::Ptr<Device> m_device;
        core::Ptr<Texture> m_currentTexture;
        ComPtr<IDXGISwapChain> m_swapchain;
    };
}