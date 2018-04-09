#include "FileSystem.h"
#include "EverydayTools/UnusedVar.h"
#include "File.h"

namespace keng::filesystem
{
    FileSystem::FileSystem() = default;
    FileSystem::~FileSystem() = default;

    void FileSystem::Initialize(const core::IApplicationPtr& app) {
        UnusedVar(app);
    }

    bool FileSystem::Update() {
        return true;
    }

    std::string_view FileSystem::GetSystemName() const {
        return SystemName();
    }

    bool FileSystem::ForEachDependency(const edt::Delegate<bool(std::string_view)>& delegate) const {
        UnusedVar(delegate);
        return false;
    }

    IFilePtr FileSystem::GetFile(const std::string_view& filename, const OpenFileParameters& params) {
        return CallAndRethrowM + [&] {
            return FilePtr::MakeInstance(filename, params);
        };
    }
}
