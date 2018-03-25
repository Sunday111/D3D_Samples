#pragma once

#include "Keng/Core/Ptr.h"
#include "Keng/Graphics/Resource/TextureView.h"

namespace keng::graphics
{
    class IDevice;
    class ISwapChain
    {
    public:
        virtual core::Ptr<ITextureView> GetBackBufferView(IDevice* device) = 0;
        virtual void* GetNativeInterface() const = 0;
        virtual void Present() = 0;
        virtual ~ISwapChain() = default;
    };
}
