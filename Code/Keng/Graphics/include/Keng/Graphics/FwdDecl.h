#pragma once

#include "Keng/ResourceSystem/FwdDecl.h"

namespace keng::graphics
{
    class IDepthStencil;
    using IDepthStencilPtr = core::Ptr<IDepthStencil>;

    class DepthStencilParameters;

    class ISwapChain;
    using ISwapChainPtr = core::Ptr<ISwapChain>;

    class IRenderTarget;
    using IRenderTargetPtr = core::Ptr<IRenderTarget>;

    class ITextureRenderTarget;
    using ITextureRenderTargetPtr = core::Ptr<ITextureRenderTarget>;

    class IWindowRenderTarget;
    using IWindowRenderTargetPtr = core::Ptr<IWindowRenderTarget>;

    class TextureRenderTargetParameters;

    class WindowRenderTargetParameters;

    class SwapChainParameters;

    class IEffect;
    using IEffectPtr = core::Ptr<IEffect>;

    class ITexture;
    using ITexturePtr = core::Ptr<ITexture>;

    class IDeviceBuffer;
    using IDeviceBufferPtr = core::Ptr<IDeviceBuffer>;

    class DeviceBufferParams;

    class IGraphicsListener;

}
