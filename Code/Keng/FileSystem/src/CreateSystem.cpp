#include "FileSystem.h"

namespace keng::filesystem
{
    IFileSystemPtr GetFileSystem() {
        static FileSystemPtr instance = FileSystemPtr::MakeInstance();
        return instance;
    }
}

extern "C"
{
    void __declspec(dllexport) __cdecl CreateSystem(void** result) {
        *result = keng::filesystem::GetFileSystem().Get();
    }
}
