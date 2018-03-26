#include "SwapChain.h"

namespace keng::graphics
{
    SwapChain::SwapChain(ID3D11Device* device, uint32_t w, uint32_t h, HWND hWnd, DXGI_FORMAT format) {
        CallAndRethrowM + [&] {
            ComPtr<IDXGIFactory> factory;
            WinAPI<char>::ThrowIfError(CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)(factory.Receive())));

            DXGI_SWAP_CHAIN_DESC scd{};
            scd.BufferCount = 1;                                    // one back buffer
            scd.BufferDesc.Format = format;                         // use 32-bit color
            scd.BufferDesc.Width = w;
            scd.BufferDesc.Height = h;
            scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;      // how swap chain is to be used
            scd.OutputWindow = hWnd;                                // the window to be used
            scd.SampleDesc.Count = 1;                               // how many multisamples
            scd.Windowed = TRUE;                                    // windowed/full-screen mode
            scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
            WinAPI<char>::ThrowIfError(factory->CreateSwapChain(device, &scd, m_swapchain.Receive()));
        };
    }

    SwapChain::~SwapChain() {
        if (m_swapchain.Get()) {
            m_swapchain->SetFullscreenState(false, nullptr);
        }
    }

    core::Ptr<ITextureView> SwapChain::GetBackBufferView(IDevice* device) {
        if (!m_renderTargetView) {
            auto backbuffer = GetBackBuffer();
            m_renderTargetView = core::Ptr<TextureView<ResourceViewType::RenderTarget>>::MakeInstance(
                (ID3D11Device*)device->GetNativeDevice(),
                backbuffer.GetTexture());
        }
        return m_renderTargetView;
    }

    void SwapChain::Present() {
        m_swapchain->Present(0, 0);
    }

    d3d_tools::Texture SwapChain::GetBackBuffer(uint32_t index) {
        return CallAndRethrowM + [&] {
            ComPtr<ID3D11Texture2D> backBuffer;
            // get the address of the back buffer
            WinAPI<char>::ThrowIfError(m_swapchain->GetBuffer(index, __uuidof(ID3D11Texture2D), (void**)backBuffer.Receive()));
            backBuffer->AddRef();
            return d3d_tools::Texture(std::move(backBuffer));
        };
    }
}
