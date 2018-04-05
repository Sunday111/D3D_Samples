#include "Resource/Font/FreeType/Library.h"
#include "EverydayTools/Exception/ThrowIfFailed.h"
#include "EverydayTools/Exception/CallAndRethrow.h"
#include "Resource/Font/FreeType/Face.h"

namespace keng::graphics::free_type
{
    Library& Library::Instance() {
        static LibraryPtr instance = LibraryPtr::MakeInstance();
        return *instance;
    }

    Library::Library() {
        CallAndRethrowM + [&]() {
            FreeTypeCall("FT_Init_FreeType", FT_Init_FreeType, &m_library);
        };
    }

    FacePtr Library::LoadFace(std::vector<uint8_t> fileData) {
        return CallAndRethrowM + [&] {
            return FacePtr::MakeInstance(*this, std::move(fileData));
        };
    }

    FT_Library Library::GetHandle() const {
        return m_library;
    }

    Library::~Library() {
        auto error = FT_Init_FreeType(&m_library);
        if (error) {
            assert(false);
        }
    }
}
