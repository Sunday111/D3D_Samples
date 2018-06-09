#pragma once

#include "FwdDecl.h"
#include "d3d11.h"
#include "WinWrappers/ComPtr.h"
#include "Keng/GPU/RenderTarget/ISwapChain.h"

namespace keng::graphics::gpu
{
    class SwapChain :
        public core::RefCountImpl<ISwapChain>
    {
    public:
        SwapChain(Device& device, const SwapChainParameters& params, window_system::IWindow& window);
        ~SwapChain();

        virtual void Present() override;
        DeviceTexture& GetCurrentTexture();
        const DeviceTexture& GetCurrentTexture() const;
        void CopyFromTexture(const DeviceTexture& texture);

    private:
        ComPtr<ID3D11Texture2D> GetBackBuffer(size_t index) const;

        DevicePtr m_device;
        TexturePtr m_currentTexture;
        ComPtr<IDXGISwapChain> m_swapchain;
    };
}