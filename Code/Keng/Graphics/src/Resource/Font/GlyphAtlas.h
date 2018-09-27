#pragma once

#include "FwdDecl.h"
#include <cstdint>
#include <memory>
#include <vector>

#include "Resource/Font/FreeType/GlyphInfo.h"
#include "EverydayTools/EnumFlag.h"

namespace keng::graphics
{
    class GlyphAtlasParameters
    {
    public:
        size_t width = 1280;
        size_t height = 720;
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
        AtlasGlyphInfo GetGlyphInfo(uint32_t unicode, const graphics::GlyphParameters& params);

        void BeginUpdate();
        void EndUpdate();

    private:
        enum StateFlag
        {
            None = 0,
            Updating = (1 << 0),
            NeedsFlush = (1 << 1)
        };

        template<typename StateFlag flag>
        bool GetFlagState() const {
            return (m_flags & flag) != StateFlag::None;
        }

        template<typename StateFlag flag, bool state>
        void SetFlagState() {
            if constexpr (state) {
                m_flags |= flag;
            } else {
                m_flags &= ~flag;
            }
        }

        using AtlasGlyphsCollection = std::vector<AtlasGlyphData>;

        AtlasGlyphData& AddGlyph(const free_type::GlyphParameters& params, AtlasGlyphsCollection::iterator insertIt);
        void CopyGlyphData(const AtlasGlyphData&);
        void FlushTexture();

        TexturePtr m_texture;
        Font* m_font;
        std::unique_ptr<uint8_t[]> m_cpuTexture;

        size_t next_x = 0;
        size_t next_y = 0;
        StateFlag m_flags;
        GlyphAtlasParameters m_parameters;
        AtlasGlyphsCollection m_glyphs;
    };
}

namespace edt
{
	template<>
	struct enable_enum_flags<keng::graphics::GlyphAtlas::StateFlag>
	{
		static constexpr bool value = true;
	};
}
