#pragma once

#include "FwdDecl.h"
#include <vector>
#include "GlyphInfo.h"

namespace keng::graphics::free_type
{
    class Face : public core::RefCountImpl<core::IRefCountObject>
    {
    public:
        Face(Library& library, std::vector<uint8_t> fileData);
        ~Face();

        GlyphData RenderGlyph(const GlyphParameters& params);

    private:
        FT_Face m_face;
        LibraryPtr m_library;
        std::vector<uint8_t> m_fileData;
    };
}