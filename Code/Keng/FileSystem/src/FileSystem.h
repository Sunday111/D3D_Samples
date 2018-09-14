#pragma once

#include "FwdDecl.h"
#include "Keng/FileSystem/IFileSystem.h"
#include "Keng/Core/System.h"

namespace keng::filesystem
{
    class FileInfo
    {
    public:
        IFilePtr file;
    };

    class FileSystem : public core::System<IFileSystem, FileSystem>
    {
    public:
        FileSystem();
        ~FileSystem();

        // IFileSystem
        virtual IFilePtr GetFile(const char* filename, const OpenFileParameters& params) override final;
    };
}
