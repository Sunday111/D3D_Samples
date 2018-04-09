#include "Resource/Font/FreeType/Face.h"
#include "EverydayTools/Exception/CallAndRethrow.h"
#include "Resource/Font/FreeType/Library.h"
#include "EverydayTools/Exception/CheckedCast.h"

namespace keng::graphics::free_type
{
    Face::Face(Library& library, std::vector<uint8_t> fileData) {
        CallAndRethrowM + [&] {
            FreeTypeCall(
                "FT_New_Memory_Face",
                FT_New_Memory_Face,
                library.GetHandle(),
                (const FT_Byte*)fileData.data(),
                edt::CheckedCast<FT_Long>(fileData.size()),
                edt::CheckedCast<FT_Long>(0),
                &m_face);

            m_fileData = std::move(fileData);
            m_library = &library;
        };
    }

    GlyphData Face::RenderGlyph(const GlyphParameters& params) {
        return CallAndRethrowM + [&] {
            // Set character size
            FreeTypeCall(
                "FT_Set_Char_Size",
                FT_Set_Char_Size,
                m_face,                                         // Handle to face object
                edt::CheckedCast<FT_F26Dot6>(params.width_pt * 64),  // char width in 1/64th points; 0 means "same as height"
                edt::CheckedCast<FT_F26Dot6>(params.height_pt * 64), // char height in 1/64th points
                edt::CheckedCast<FT_UInt>(params.x_dpi),             // horizontal device resolution
                edt::CheckedCast<FT_UInt>(params.y_dpi)              // vertical device resolution
            );

            auto glyphIndex = FT_Get_Char_Index(m_face, params.unicode);

            FreeTypeCall(
                "FT_Load_Glyph",
                FT_Load_Glyph,
                m_face,
                glyphIndex,
                FT_LOAD_DEFAULT);

            FreeTypeCall(
                "FT_Render_Glyph",
                FT_Render_Glyph,
                m_face->glyph,
                FT_RENDER_MODE_NORMAL
            );

            auto slot = m_face->glyph;
            GlyphData data;

            // Which is ok for the first 127 ASCII...
            data.unicode = params.unicode;

            {// Copy metrics
                auto metricToPixel = [](auto value) {
                    assert(value % 64 == 0);
                    return value / 64;
                };

                auto& metrics = slot->metrics;
                data.width = metricToPixel(metrics.width);
                data.height = metricToPixel(metrics.height);
                data.advanceX = metricToPixel(metrics.horiAdvance);
                data.advanceY = metricToPixel(metrics.vertAdvance);
                data.horizontalBearingX = metricToPixel(metrics.horiBearingX);
                data.horizontalBearingY = metricToPixel(metrics.horiBearingY);
                data.verticalBearingX = metricToPixel(metrics.vertBearingX);
                data.verticalBearingY = metricToPixel(metrics.vertBearingY);
            }

            {// Copy buffer
                size_t w = data.width;
                size_t h = data.height;
                data.buffer.reset(new uint8_t[w * h]);
                for (size_t y = 0; y < h; ++y) {
                    auto offset = w * y;
                    void* toRow = data.buffer.get() + offset;
                    void* fromRow = slot->bitmap.buffer + offset;
                    std::memcpy(toRow, fromRow, w);
                }
            }

            return data;
        };
    }

    Face::~Face() {
        auto err = FT_Done_Face(m_face);
        assert(err == 0);
    }
}
