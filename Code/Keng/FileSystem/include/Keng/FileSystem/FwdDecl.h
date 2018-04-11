#pragma once

#include "Keng/FileSystem/Common.h"
#include "Keng/Core/Ptr.h"

namespace keng::filesystem
{
    class IFileSystem;
    using IFileSystemPtr = core::Ptr<IFileSystem>;

    class IFile;
    using IFilePtr = core::Ptr<IFile>;

    class OpenFileParameters;
}
