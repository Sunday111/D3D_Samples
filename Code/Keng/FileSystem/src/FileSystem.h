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
        virtual void Shutdown() override;
        virtual const char* GetSystemName() const override;
        virtual bool ForEachDependency(const edt::Delegate<bool(const char*)>& delegate) const override;

        // IGlobalSystem
        virtual void AssignId(size_t id) override;

        // IFileSystem
        virtual IFilePtr GetFile(const char* filename, const OpenFileParameters& params) override;
    };
}
