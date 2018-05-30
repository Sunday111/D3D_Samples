#pragma once

#include "Keng/Graphics/FwdDecl.h"

namespace keng::graphics
{
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

    class GraphicsSystem;
    using GraphicsSystemPtr = core::Ptr<GraphicsSystem>;

    class Font;
    using FontPtr = core::Ptr<Font>;

    class FontFabric;
    using FontFabricPtr = core::Ptr<FontFabric>;

    class GlyphAtlas;
    using GlyphAtlasPtr = core::Ptr<GlyphAtlas>;

    class ResourceFabricRegisterer;

    class Device;
    using DevicePtr = core::Ptr<Device>;
}
