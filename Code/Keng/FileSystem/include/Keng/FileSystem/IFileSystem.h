#pragma once

#include "Keng/Core/ISystem.h"
#include "Keng/FileSystem/FwdDecl.h"
#include <string_view>

namespace keng::filesystem
{
    class IFileSystem : public core::ISystem
    {
    public:
        static std::string_view SystemName() { return "KengFileSystem"; }
        virtual IFilePtr GetFile(const std::string_view& filename, const OpenFileParameters& params) = 0;
        virtual ~IFileSystem() = default;
    };
}
