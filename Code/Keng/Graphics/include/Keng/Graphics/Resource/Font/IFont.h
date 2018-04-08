#pragma once

#include <string_view>
#include "Keng/Graphics/FwdDecl.h"
#include "Keng/ResourceSystem/IResource.h"

namespace keng::graphics
{
    class IFont : public resource::IResource
    {
    public:
        virtual AtlasGlyphInfo RequestGlyphInfo(uint32_t unicode, IDevice&, const GlyphParameters&) = 0;
        virtual ~IFont() = default;
    };
}

