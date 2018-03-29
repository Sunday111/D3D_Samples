#include "SwapChain.h"
#include "Keng/Graphics/Device.h"
#include "Keng/WindowSystem/IWindow.h"
#include "D3D_11/EnumConverter.h"
#include "Resource/Texture/Texture.h"

namespace keng::graphics
{
    SwapChain::SwapChain(Device& device, const SwapChainParameters& params) {
        CallAndRethrowM + [&] {
            m_device = &device;
            ComPtr<IDXGIFactory> factory;
            WinAPI<char>::ThrowIfError(CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)(factory.Receive())));

            DXGI_SWAP_CHAIN_DESC scd{};
            scd.BufferCount = 1;                                                                    // one back buffer
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

    core::Ptr<ITextureView> SwapChain::GetBackBufferView() {
        auto backbuffer = std::static_pointer_cast<Texture>(GetBackBuffer());
        return backbuffer->GetView<ResourceViewType::RenderTarget>(m_device->GetDevice().Get(), backbuffer->GetFormat());
    }

    void SwapChain::Present() {
        m_swapchain->Present(0, 0);
    }

    core::Ptr<ITexture> SwapChain::GetBackBuffer(uint32_t index) {
        return CallAndRethrowM + [&] {
            return core::Ptr<Texture>::MakeInstance(*m_device, GetTexture(index));
        };
    }

    ComPtr<ID3D11Texture2D> SwapChain::GetTexture(uint32_t index) {
        return CallAndRethrowM + [&] {
            ComPtr<ID3D11Texture2D> backBuffer;
            WinAPI<char>::ThrowIfError(m_swapchain->GetBuffer(index, __uuidof(ID3D11Texture2D), (void**)backBuffer.Receive()));
            backBuffer->AddRef();
            return backBuffer;
        };
    }

    void SwapChain::CopyFromTexture(const core::Ptr<Texture>& texture, uint32_t index) {
        return CallAndRethrowM + [&] {
            texture->CopyTo(GetTexture(index));
        };
    }
}
