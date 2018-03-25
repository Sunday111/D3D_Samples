#pragma once

#include "D3D_Tools/SwapChain.h"
#include "Keng/Graphics/ISwapChain.h"
#include "Keng/Graphics/Device.h"

namespace keng::graphics
{

    class SwapChain :
        public ISwapChain,
        public d3d_tools::SwapChain
    {
    public:
        using d3d_tools::SwapChain::SwapChain;

        virtual core::Ptr<ITextureView> GetBackBufferView(IDevice* device) override {
            if (!m_renderTargetView) {
                auto backbuffer = GetBackBuffer();
                m_renderTargetView = core::Ptr<TextureView<ResourceViewType::RenderTarget>>::MakeInstance(
                    (ID3D11Device*)device->GetNativeDevice(),
                    backbuffer.GetTexture());
            }
            return m_renderTargetView;
        }
        
        void* GetNativeInterface() const {
            return GetInterface();
        }

        virtual void Present() override {
            d3d_tools::SwapChain::Present();
        }

    private:
        core::Ptr<TextureView<ResourceViewType::RenderTarget>> m_renderTargetView;
    };
}