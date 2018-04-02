#include "SwapChain.h"
#include "Keng/Graphics/Device.h"
#include "Keng/WindowSystem/IWindow.h"
#include "D3D_11/EnumConverter.h"
#include "Resource/Texture/Texture.h"
#include "Keng/Graphics/RenderTarget/SwapChainParameters.h"

namespace keng::graphics
{
    SwapChain::SwapChain(Device& device, const SwapChainParameters& params) {
        CallAndRethrowM + [&] {
            m_device = &device;
            ComPtr<IDXGIFactory> factory;
            WinAPI<char>::ThrowIfError(CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)(factory.Receive())));

            DXGI_SWAP_CHAIN_DESC scd{};
            scd.BufferCount = static_cast<unsigned>(params.buffers);                                // one back buffer
            scd.BufferDesc.Format = d3d::ConvertTextureFormat(params.format);
            scd.BufferDesc.Width = static_cast<unsigned>(params.width);
            scd.BufferDesc.Height = static_cast<unsigned>(params.height);
            scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;                                      // how swap chain is to be used
            scd.OutputWindow = (HWND)params.window->GetNativeHandle();                              // the window to be used
            scd.SampleDesc.Count = 1;                                                               // how many multisamples
            scd.Windowed = TRUE;                                                                    // windowed/full-screen mode
            scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
            WinAPI<char>::ThrowIfError(factory->CreateSwapChain(device.GetDevice().Get(), &scd, m_swapchain.Receive()));
        };
    }

    SwapChain::~SwapChain() {
        if (m_swapchain.Get()) {
            m_swapchain->SetFullscreenState(false, nullptr);
        }
    }

    void SwapChain::Present() {
        m_swapchain->Present(0, 0);
    }

    TexturePtr SwapChain::GetCurrentTexture() {
        if (!m_currentTexture) {
            auto rawTexture = GetBackBuffer(0);
            m_currentTexture = TexturePtr::MakeInstance(*m_device, rawTexture);
        }

        return m_currentTexture;
    }

    ComPtr<ID3D11Texture2D> SwapChain::GetBackBuffer(uint32_t index) {
        return CallAndRethrowM + [&] {
            ComPtr<ID3D11Texture2D> backBuffer;
            WinAPI<char>::ThrowIfError(m_swapchain->GetBuffer(index, __uuidof(ID3D11Texture2D), (void**)backBuffer.Receive()));
            backBuffer->AddRef();
            return backBuffer;
        };
    }

    void SwapChain::CopyFromTexture(const TexturePtr& texture) {
        return CallAndRethrowM + [&] {
            texture->CopyTo(GetCurrentTexture());
        };
    }
}
