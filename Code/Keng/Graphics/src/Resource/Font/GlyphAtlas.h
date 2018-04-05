#pragma once

#include "FwdDecl.h"
#include <cstdint>
#include <memory>
#include <vector>

#include "Resource/Font/FreeType/GlyphInfo.h"

namespace keng::graphics
{
    class GlyphAtlasParameters
    {
    public:
        uint32_t width = 300;
        uint32_t height = 300;
    };

    class AtlasGlyphData
    {
    public:
        // Position in atlas texture
        size_t x = 0;
        size_t y = 0;

        // Glyph only data
        free_type::GlyphData data;
    };

    class GlyphAtlas : public core::RefCountImpl<core::IRefCountObject>
    {
    public:
        GlyphAtlas(Device& device, const GlyphAtlasParameters& params, Font& font);
        ~GlyphAtlas();

        TexturePtr GetTexture() const;

    private:
        AtlasGlyphData& AddGlyph(const free_type::GlyphParameters& params);

        TexturePtr m_texture;
        FontPtr m_font;
        std::unique_ptr<uint8_t[]> m_cpuTexture;

        size_t next_x = 0;
        size_t next_y = 0;
        GlyphAtlasParameters m_parameters;
        std::vector<AtlasGlyphData> m_glyphs;
    };
}
