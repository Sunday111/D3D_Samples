#pragma once

#include "Keng/WindowSystem/FwdDecl.h"

namespace keng::graphics_api
{
    class DepthStencilParameters;
    class IDepthStencil;
    using IDepthStencilPtr = core::Ptr<IDepthStencil>;

    class SwapChainParameters;
    class ISwapChain;
    using ISwapChainPtr = core::Ptr<ISwapChain>;

    class IRenderTarget;
    using IRenderTargetPtr = core::Ptr<IRenderTarget>;

    class TextureRenderTargetParameters;
    class ITextureRenderTarget;
    using ITextureRenderTargetPtr = core::Ptr<ITextureRenderTarget>;

    class WindowRenderTargetParameters;
    class IWindowRenderTarget;
    using IWindowRenderTargetPtr = core::Ptr<IWindowRenderTarget>;

    class TextureParameters;
    class ITexture;
    using ITexturePtr = core::Ptr<ITexture>;

    class DeviceBufferParameters;
    class IDeviceBuffer;
    using IDeviceBufferPtr = core::Ptr<IDeviceBuffer>;

    class DeviceBufferMapper;

    class IGraphicsListener;

    class ISampler;
    using ISamplerPtr = core::Ptr<ISampler>;

    class SamplerParameters;
    class ViewportParameters;

    class IAnnotation;
    using IAnnotationPtr = core::Ptr<IAnnotation>;

    class DeviceParameters;
    class IDevice;
    using IDevicePtr = core::Ptr<IDevice>;

    class IGraphicsAPISystem;
    using IGraphicsAPISystemPtr = core::Ptr<IGraphicsAPISystem>;

    class IInputLayout;
    using IInputLayoutPtr = core::Ptr<IInputLayout>;

    class ShaderParameters;
    class IShader;
    using IShaderPtr = core::Ptr<IShader>;
}
