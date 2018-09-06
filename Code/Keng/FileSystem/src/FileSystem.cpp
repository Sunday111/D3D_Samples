#include "FileSystem.h"
#include "EverydayTools/UnusedVar.h"
#include "EverydayTools/Exception/ThrowIfFailed.h"
#include "EverydayTools/Exception/CallAndRethrow.h"
#include "File.h"

namespace keng::filesystem
{
    namespace
    {
        static int globalSystemId = -1;
    }

    FileSystem::FileSystem() = default;
    FileSystem::~FileSystem() = default;

    IFilePtr FileSystem::GetFile(const char* filename, const OpenFileParameters& params) {
        return CallAndRethrowM + [&] {
            return FilePtr::MakeInstance(filename, params);
        };
    }
}
