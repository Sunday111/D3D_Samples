#pragma once

#include "Keng/Core/Ptr.h"
#include "Keng/Core/IRefCountObject.h"
#include "Keng/Graphics/Resource/TextureView.h"

namespace keng::window_system
{
    class IWindow;
}

namespace keng::graphics
{
    class SwapChainParameters
    {
    public:
        uint32_t width = 0;
        uint32_t height = 0;
        d3d_tools::TextureFormat format = d3d_tools::TextureFormat::R8_G8_B8_A8_UNORM;
        window_system::IWindow* window;
    };

    class IDevice;
    class ISwapChain : public core::IRefCountObject
    {
    public:
        virtual core::Ptr<ITextureView> GetBackBufferView() = 0;
        virtual void Present() = 0;
        virtual ~ISwapChain() = default;
    };
}
