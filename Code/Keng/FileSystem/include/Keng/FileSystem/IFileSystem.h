#pragma once

#include "Keng/Core/IGlobalSystem.h"
#include "Keng/FileSystem/FwdDecl.h"

namespace keng::filesystem
{
    class IFileSystem : public core::IGlobalSystem
    {
    public:
        static const char* SystemName() { return "KengFileSystem"; }
        virtual IFilePtr GetFile(const char* filename, const OpenFileParameters& params) = 0;
        virtual ~IFileSystem() = default;
    };
}
