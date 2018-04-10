#pragma once

#include "EverydayTools/EnumFlag.h"
#include "Keng/FileSystem/FwdDecl.h"

namespace keng::filesystem
{
    enum class FileAccessFlags
    {
        None,
        Read = (1 << 1),
        Write = (1 << 2)
    };

    EDT_ENUM_FLAG_OPERATORS(FileAccessFlags);

    class OpenFileParameters
    {
    public:
        FileAccessFlags accessFlags = FileAccessFlags::Read;
    };
}
