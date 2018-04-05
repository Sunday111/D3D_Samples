#pragma once

#include <cassert>
#include <memory>
#include <vector>

#include "EverydayTools/Exception/CallAndRethrow.h"
#include "FwdDecl.h"
#include "Resource/Font/FreeType/FwdDecl.h"

namespace keng::graphics::free_type
{
    class Library : public core::RefCountImpl<core::IRefCountObject>
    {
    public:
        static Library& Instance();

        Library();
        ~Library();

        FacePtr LoadFace(std::vector<uint8_t> fileData);
        FT_Library GetHandle() const;

    private:
        FT_Library m_library;
        FT_Face m_face;
    };
}
