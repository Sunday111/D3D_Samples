#pragma once

#include <cassert>

#include "EverydayTools/Exception/CallAndRethrow.h"

#include <ft2build.h>
#include FT_FREETYPE_H

namespace render_text_sample::free_type {
    struct LetterInfo
    {
        uint32_t unicode;

        size_t horizontalBearingX;
        size_t horizontalBearingY;

        size_t verticalBearingX;
        size_t verticalBearingY;

        size_t width;
        size_t height;

        size_t advanceX;
        size_t advanceY;

        std::unique_ptr<uint8_t[]> buffer;
    };

    using LetterInfoPtr = std::unique_ptr<LetterInfo>;

    class Library
    {
    public:
        Library();
        ~Library();

        void LoadFace();

        FT_Library m_library;
        FT_Face m_face;
    };
}
