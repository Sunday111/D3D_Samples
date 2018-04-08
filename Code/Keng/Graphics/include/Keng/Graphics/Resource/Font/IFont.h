#pragma once

#include <string_view>
#include "Keng/Graphics/FwdDecl.h"
#include "Keng/ResourceSystem/IResource.h"
#include "EverydayTools/Array/ArrayView.h"
#include "EverydayTools/Delegate.h"

namespace keng::graphics
{
    class IFont : public resource::IResource
    {
    public:
        virtual AtlasGlyphInfo RequestGlyphInfo(uint32_t unicode, IDevice&, const GlyphParameters&) = 0;
        virtual void RequestGlyphsInfo(edt::DenseArrayView<const uint32_t> unicodes, IDevice&, const GlyphParameters&, edt::Delegate<void(const AtlasGlyphInfo&)> delegate) = 0;
        virtual ~IFont() = default;
    };
}

