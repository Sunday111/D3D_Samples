#pragma once

#include "Keng/Graphics/Resource/Font/IFont.h"
#include "Keng/ResourceSystem/IResourceSystem.h"
#include "FwdDecl.h"
#include <string_view>
#include "FreeType/Face.h"
#include <string>
#include <vector>

namespace keng::graphics
{

    class Font : public core::RefCountImpl<IFont>
    {
    public:
        Font(std::vector<uint8_t> fileData, resource::IResourceSystem& resourceSystem);
        ~Font();

        // IFont
        virtual AtlasGlyphInfo RequestGlyphInfo(uint32_t unicode, IDevice&, const GlyphParameters&) override;

        free_type::FacePtr GetFace() const;
        resource::IResourceSystemPtr GetResourceSystem() const;
        GlyphAtlasPtr FindAtlas(IDevice& device) const;

    private:
        free_type::FacePtr m_face;
        resource::IResourceSystemPtr m_resourceSystem;
        std::vector<GlyphAtlasPtr> m_atlases;
    };
}
