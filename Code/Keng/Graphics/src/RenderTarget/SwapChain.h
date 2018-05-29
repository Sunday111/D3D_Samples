#pragma once

#include "FwdDecl.h"
#include "d3d11.h"
#include "WinWrappers/ComPtr.h"
#include "Keng/Graphics/RenderTarget/ISwapChain.h"

namespace keng::graphics
{
    class SwapChain :
        public core::RefCountImpl<ISwapChain>
    {
    public:
        SwapChain(Device& device, const SwapChainParameters& params);
        ~SwapChain();

        virtual void Present() override;
        TexturePtr GetCurrentTexture();
        void CopyFromTexture(const TexturePtr& texture);

    private:
        ComPtr<ID3D11Texture2D> GetBackBuffer(size_t index);

        DevicePtr m_device;
        TexturePtr m_currentTexture;
        ComPtr<IDXGISwapChain> m_swapchain;
    };
}