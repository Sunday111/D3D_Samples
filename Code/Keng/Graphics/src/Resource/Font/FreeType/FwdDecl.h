#pragma once

#include "Keng/Core/FwdDecl.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#include <string_view>

namespace keng::graphics::free_type
{
    class Face;
    using FacePtr = core::Ptr<Face>;

    class Library;
    using LibraryPtr = core::Ptr<Library>;

    template<typename... FnArgs, typename... Args>
    void FreeTypeCall(std::string_view title, FT_Error(*pfn)(FnArgs...), Args... args) {
        auto error = pfn(args...);
        edt::ThrowIfFailed(error == 0, title, " failed");
    }

}