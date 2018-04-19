#include "FileSystem.h"
#include "EverydayTools/UnusedVar.h"
#include "EverydayTools/Exception/ThrowIfFailed.h"
#include "File.h"

namespace keng::filesystem
{
    namespace
    {
        static int globalSystemId = -1;
    }

    size_t IFileSystem::GetGlobalSystemId() {
        edt::ThrowIfFailed(globalSystemId >= 0, "Trying to use uninitialized global system!");
        return static_cast<size_t>(globalSystemId);
    }

    FileSystem::FileSystem() = default;
    FileSystem::~FileSystem() = default;

    void FileSystem::Initialize(const core::IApplicationPtr& app) {
        UnusedVar(app);
    }

    bool FileSystem::Update() {
        return true;
    }

    void FileSystem::Shutdown() {

    }

    std::string_view FileSystem::GetSystemName() const {
        return SystemName();
    }

    bool FileSystem::ForEachDependency(const edt::Delegate<bool(std::string_view)>& delegate) const {
        UnusedVar(delegate);
        return false;
    }

    void FileSystem::AssignId(size_t id) {
        edt::ThrowIfFailed(globalSystemId < 0, "Trying to assign global system id twice!");
        globalSystemId = static_cast<int>(id);
    }

    IFilePtr FileSystem::GetFile(const std::string_view& filename, const OpenFileParameters& params) {
        return CallAndRethrowM + [&] {
            return FilePtr::MakeInstance(filename, params);
        };
    }
}
