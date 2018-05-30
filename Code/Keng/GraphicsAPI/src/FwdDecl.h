#pragma once

#include "Keng/GraphicsAPI/FwdDecl.h"

namespace keng::graphics_api
{
    class DepthStencil;
    using DepthStencilPtr = core::Ptr<DepthStencil>;

    class SwapChain;
    using SwapChainPtr = core::Ptr<SwapChain>;

    class TextureRenderTarget;
    using TextureRenderTargetPtr = core::Ptr<TextureRenderTarget>;

    class WindowRenderTarget;
    using WindowRenderTargetPtr = core::Ptr<WindowRenderTarget>;

    class Texture;
    using TexturePtr = core::Ptr<Texture>;

    class DeviceBuffer;
    using DeviceBufferPtr = core::Ptr<DeviceBuffer>;

    class Device;
    using DevicePtr = core::Ptr<Device>;

    class Sampler;
    using SamplerPtr = core::Ptr<Sampler>;

    class Annotation;
    using AnnotationPtr = core::Ptr<Annotation>;

    class GraphicsAPISystem;
    using GraphicsSystemPtr = core::Ptr<GraphicsAPISystem>;

    class InputLayout;
    using InputLayoutPtr = core::Ptr<InputLayout>;

    class ResourceFabricRegisterer;
}
