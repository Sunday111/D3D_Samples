#pragma once

#include "EverydayTools/EnumFlag.h"
#include "Keng/FileSystem/FwdDecl.h"

namespace keng::filesystem
{
    enum class FileAccessFlags
    {
        None,
        Read = (1 << 0),
        Write = (1 << 1)
    };

    class OpenFileParameters
    {
    public:
        FileAccessFlags accessFlags = FileAccessFlags::Read;
    };
}


namespace edt
{
	template<>
	struct enable_enum_flags<keng::filesystem::FileAccessFlags>
	{
		static constexpr bool value = true;
	};
}