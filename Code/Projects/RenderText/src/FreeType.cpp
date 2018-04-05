#include "FreeType.h"
#include "EverydayTools/Exception/ThrowIfFailed.h"
#include <string_view>
#include "Keng/Base/Serialization/ReadFileToBuffer.h"
#include "EverydayTools/Geom/Vector.h"
#include "Windows.h"
#include <algorithm>
#include <memory>
#include <fstream>

namespace render_text_sample::free_type
{
    template<typename... FnArgs, typename... Args>
    void FreeTypeCall(std::string_view title, FT_Error (*pfn)(FnArgs...), Args... args) {
        auto error = pfn(args...);
        edt::ThrowIfFailed(error == 0, title, " failed");
    }

    Library::Library() {
        CallAndRethrowM + [&]() {
            FreeTypeCall("FT_Init_FreeType", FT_Init_FreeType, &m_library);
        };
    }

    Library::~Library() {
        auto error = FT_Init_FreeType(&m_library);
        if (error) {
            assert(false);
        }
    }

    void Library::LoadFace() {
        CallAndRethrowM + [&]() {
            FT_Face face;

            auto buffer = keng::ReadFileToBuffer("Assets/ttf/OpenSans-Regular.ttf");
            FreeTypeCall(
                "FT_New_Memory_Face",
                FT_New_Memory_Face,
                m_library,
                (const FT_Byte*)buffer.data(),
                static_cast<FT_Long>(buffer.size()),
                static_cast<FT_Long>(0),
                &face);

            // 1 point equals 1/72th of an inch in digital typography
            // Size of one point in inches
            constexpr auto pointSize = 1.f / 72.f; 

            // Device horizontal resolution
            size_t x_dpi = 300;

            // Device vertical resolution
            size_t y_dpi = 300;

            // Height of character in points
            size_t height_pt = 20;
            size_t width_pt = height_pt;

            // Max height expected for one character
            size_t height_px = static_cast<size_t>(height_pt * pointSize * y_dpi + 1);
            size_t width_px = static_cast<size_t>(width_pt * pointSize * x_dpi + 1);

            // Set character size
            FreeTypeCall(
                "FT_Set_Char_Size",
                FT_Set_Char_Size,
                face,                                      // Handle to face object
                static_cast<FT_F26Dot6>(width_pt * 64),    // char width in 1/64th points; 0 means "same as height"
                static_cast<FT_F26Dot6>(height_pt * 64),   // char height in 1/64th points
                static_cast<FT_UInt>(x_dpi),               // horizontal device resolution
                static_cast<FT_UInt>(y_dpi)                // vertical device resolution
            );


            std::vector<LetterInfoPtr> letterInfos;
            std::string alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefgijklmnopqrstuvwxyz1234567890";
            for(size_t letter_index = 0; letter_index < alphabet.size(); ++letter_index) {
                auto letter = alphabet[letter_index];
                auto glyphIndex = FT_Get_Char_Index(face, letter);

                FreeTypeCall(
                    "FT_Load_Glyph",
                    FT_Load_Glyph,
                    face,
                    glyphIndex,
                    FT_LOAD_DEFAULT);

                FreeTypeCall(
                    "FT_Render_Glyph",
                    FT_Render_Glyph,
                    face->glyph,
                    FT_RENDER_MODE_NORMAL
                );

                auto slot = face->glyph;
                auto info = std::make_unique<LetterInfo>();

                // Which is ok for the first 127 ASCII...
                info->unicode = letter;

                {// Copy metrics
                    auto metricToPixel = [](auto value) {
                        assert(value % 64 == 0);
                        return value / 64;
                    };

                    auto& metrics = slot->metrics;
                    info->width                 = metricToPixel(metrics.width);
                    info->height                = metricToPixel(metrics.height);
                    info->advanceX              = metricToPixel(metrics.horiAdvance);
                    info->advanceY              = metricToPixel(metrics.vertAdvance);
                    info->horizontalBearingX    = metricToPixel(metrics.horiBearingX);
                    info->horizontalBearingY    = metricToPixel(metrics.horiBearingY);
                    info->verticalBearingX      = metricToPixel(metrics.vertBearingX);
                    info->verticalBearingY      = metricToPixel(metrics.vertBearingY);
                }

                {// Copy buffer
                    size_t w = info->width;
                    size_t h = info->height;
                    info->buffer.reset(new uint8_t[w * h]);
                    for (size_t y = 0; y < h; ++y) {
                        auto offset = w * y;
                        void* toRow = info->buffer.get() + offset;
                        void* fromRow = slot->bitmap.buffer + offset;
                        std::memcpy(toRow, fromRow, w);
                    }
                }

                {
                    size_t w = info->width;
                    size_t h = info->height;
                    std::ofstream file;
                    file.open(std::string() + letter + ".txt");
                    for (size_t y = 0; y < h; ++y) {
                        for (size_t x = 0; x < w; ++x) {
                            auto val = info->buffer[w * y + x];
                            auto c = val ? '1' : ' ';
                            file.put(c);
                        }

                        file << std::endl;
                    }
                }

                letterInfos.push_back(std::move(info));
            }

            assert(false);
        };
    }

}

