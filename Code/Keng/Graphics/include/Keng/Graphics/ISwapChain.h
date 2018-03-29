#pragma once

#include "Keng/Core/Ptr.h"
#include "Keng/Core/IRefCountObject.h"
#include "Keng/Graphics/FragmentFormat.h"

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
        FragmentFormat format = FragmentFormat::R8_G8_B8_A8_UNORM;
        window_system::IWindow* window;
    };

    class IDevice;
    class ITextureView;

    class ISwapChain : public core::IRefCountObject
    {
    public:
        virtual core::Ptr<ITextureView> GetBackBufferView() = 0;
        virtual void Present() = 0;
        virtual ~ISwapChain() = default;
    };
}
