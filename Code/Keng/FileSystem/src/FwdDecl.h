#pragma once
#include "Keng/FileSystem/FwdDecl.h"

namespace keng::filesystem
{
    class FileSystem;
    using FileSystemPtr = core::Ptr<FileSystem>;

    class File;
    using FilePtr = core::Ptr<File>;
}
