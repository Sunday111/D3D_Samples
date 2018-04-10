#pragma once
#include "FwdDecl.h"
#include "Keng/FileSystem/IFileSystem.h"

namespace keng::filesystem
{
    class FileInfo
    {
    public:
        IFilePtr file;
    };

    class FileSystem : public core::RefCountImpl<IFileSystem>
    {
    public:
        FileSystem();
        ~FileSystem();

        // ISystem
        virtual void Initialize(const core::IApplicationPtr& app) override;
        virtual bool Update() override;
        virtual std::string_view GetSystemName() const override;
        virtual bool ForEachDependency(const edt::Delegate<bool(std::string_view)>& delegate) const override;

        // IFileSystem
        virtual IFilePtr GetFile(const std::string_view& filename, const OpenFileParameters& params) override;

    private:
    };
}
