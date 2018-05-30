#pragma once

#include "Keng/GPU/FwdDecl.h"
#include "Keng/ResourceSystem/FwdDecl.h"
#include "Keng/WindowSystem/FwdDecl.h"

namespace keng::graphics
{
    class IEffect;
    using IEffectPtr = core::Ptr<IEffect>;

    class ITexture;
    using ITexturePtr = core::Ptr<ITexture>;

    class IGraphicsListener;

    class IGraphicsSystem;
    using IGraphicsSystemPtr = core::Ptr<IGraphicsSystem>;

    class IFont;
    using IFontPtr = core::Ptr<IFont>;

    class IDevice;
    using IDevicePtr = core::Ptr<IDevice>;

    class GlyphParameters;
    class GlyphInfo;
    class AtlasGlyphInfo;
}
