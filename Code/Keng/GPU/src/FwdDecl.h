#pragma once

#include "Keng/GPU/FwdDecl.h"

namespace keng::graphics::gpu
{
    class DepthStencil;
    using DepthStencilPtr = core::Ptr<DepthStencil>;

    class SwapChain;
    using SwapChainPtr = core::Ptr<SwapChain>;

    class TextureRenderTarget;
    using TextureRenderTargetPtr = core::Ptr<TextureRenderTarget>;

    class WindowRenderTarget;
    using WindowRenderTargetPtr = core::Ptr<WindowRenderTarget>;

    class DeviceTexture;
    using TexturePtr = core::Ptr<DeviceTexture>;

    class DeviceBuffer;
    using DeviceBufferPtr = core::Ptr<DeviceBuffer>;

    class Device;
    using DevicePtr = core::Ptr<Device>;

    class Sampler;
    using SamplerPtr = core::Ptr<Sampler>;

    class Annotation;
    using AnnotationPtr = core::Ptr<Annotation>;

    class GPUSystem;
    using GraphicsSystemPtr = core::Ptr<GPUSystem>;

    class InputLayout;
    using InputLayoutPtr = core::Ptr<InputLayout>;

    class ResourceFabricRegisterer;
}
