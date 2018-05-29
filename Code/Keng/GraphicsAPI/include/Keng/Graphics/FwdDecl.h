#pragma once

#include "Keng/WindowSystem/FwdDecl.h"

namespace keng::graphics_api
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

    class DeviceBufferMapper;

    class DeviceBufferParams;

    class IGraphicsListener;

    class TextureParameters;

    class ISampler;
    using ISamplerPtr = core::Ptr<ISampler>;

    class SamplerParameters;
    class ViewportParameters;

    class IAnnotation;
    using IAnnotationPtr = core::Ptr<IAnnotation>;

    class IDevice;
    using IDevicePtr = core::Ptr<IDevice>;

    class IGraphicsAPISystem;
    using IGraphicsSystemPtr = core::Ptr<IGraphicsAPISystem>;

    class IFont;
    using IFontPtr = core::Ptr<IFont>;

    class GlyphParameters;
    class GlyphInfo;
    class AtlasGlyphInfo;
    class DeviceParameters;
}
