#pragma once

#include "Keng/Core/IGlobalSystem.h"
#include "Keng/FileSystem/FwdDecl.h"
#include <string_view>

namespace keng::filesystem
{
    class IFileSystem : public core::IGlobalSystem
    {
    public:
        static KENG_FILESYSTEM_API size_t GetGlobalSystemId();
        static std::string_view SystemName() { return "KengFileSystem"; }
        virtual IFilePtr GetFile(const std::string_view& filename, const OpenFileParameters& params) = 0;
        virtual ~IFileSystem() = default;
    };
}
