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

    class Effect;
    using EffectPtr = core::Ptr<Effect>;

    class EffectFabric;
    using EffectFabricPtr = core::Ptr<EffectFabric>;

    class ShaderFabric;
    using ShaderFabricPtr = core::Ptr<ShaderFabric>;

    class ShaderTemplate;
    using ShaderTemplatePtr = core::Ptr<ShaderTemplate>;

    class ShaderTemplateFabric;
    using ShaderTemplateFabricPtr = core::Ptr<ShaderTemplateFabric>;

    class Texture;
    using TexturePtr = core::Ptr<Texture>;

    class TextureFabric;
    using TextureFabricPtr = core::Ptr<TextureFabric>;

    class TextureParameters;

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

    class Font;
    using FontPtr = core::Ptr<Font>;

    class FontFabric;
    using FontFabricPtr = core::Ptr<FontFabric>;

    class GlyphAtlas;
    using GlyphAtlasPtr = core::Ptr<GlyphAtlas>;

    class ResourceFabricRegisterer;
}
