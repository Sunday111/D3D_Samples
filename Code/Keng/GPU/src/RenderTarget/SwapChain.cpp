#include "SwapChain.h"
#include "Device.h"
#include "Keng/WindowSystem/IWindow.h"
#include "EnumConverter.h"
#include "Resource/Texture/Texture.h"
#include "Keng/GraphicsCommon/SwapChainParameters.h"
#include "EverydayTools/Exception/CheckedCast.h"

namespace keng::graphics::gpu
{
    SwapChain::SwapChain(Device& device, const SwapChainParameters& params, window_system::IWindow& window) {
        CallAndRethrowM + [&] {
            m_device = &device;
            ComPtr<IDXGIFactory> factory;
            WinAPI<char>::ThrowIfError(CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)(factory.Receive())));

            DXGI_SWAP_CHAIN_DESC scd{};
            scd.BufferCount = edt::CheckedCast<uint32_t>(params.buffers);                           // one back buffer
            scd.BufferDesc.Format = ConvertTextureFormat(params.format);
            scd.BufferDesc.Width = edt::CheckedCast<uint32_t>(params.width);
            scd.BufferDesc.Height = edt::CheckedCast<uint32_t>(params.height);
            scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;                                      // how swap chain is to be used
            scd.OutputWindow = (HWND)window.GetNativeHandle();                                      // the window to be used
            scd.SampleDesc.Count = 1;                                                               // how many multisamples
            scd.Windowed = TRUE;                                                                    // windowed/full-screen mode
            scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
            WinAPI<char>::ThrowIfError(factory->CreateSwapChain(device.GetDevice().Get(), &scd, m_swapchain.Receive()));

            auto rawTexture = GetBackBuffer(0);
            m_currentTexture = TexturePtr::MakeInstance(*m_device, rawTexture);
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

    Texture& SwapChain::GetCurrentTexture() {
        return *m_currentTexture;
    }

    const Texture& SwapChain::GetCurrentTexture() const {
        return *m_currentTexture;
    }

    ComPtr<ID3D11Texture2D> SwapChain::GetBackBuffer(size_t index) const {
        return CallAndRethrowM + [&] {
            ComPtr<ID3D11Texture2D> backBuffer;
            WinAPI<char>::ThrowIfError(m_swapchain->GetBuffer(edt::CheckedCast<uint32_t>(index), __uuidof(ID3D11Texture2D), (void**)backBuffer.Receive()));
            backBuffer->AddRef();
            return backBuffer;
        };
    }

    void SwapChain::CopyFromTexture(const Texture& texture) {
        return CallAndRethrowM + [&] {
            texture.CopyTo(GetCurrentTexture());
        };
    }
}
