#pragma once

#include "FwdDecl.h"
#include "Resource/Font/FreeType/FwdDecl.h"
#include "Keng/FileSystem/FwdDecl.h"
#include "Keng/ResourceSystem/IResourceFabric.h"

namespace keng::graphics
{
    class FontFabric :
        public core::RefCountImpl<resource::IResourceFabric>
    {
    public:
        FontFabric(const filesystem::IFileSystemPtr& fileSystem);
        ~FontFabric();

        virtual const char* GetNodeName() const override final;
        virtual const char* GetResourceType() const override final;
        virtual resource::IResourcePtr LoadResource(resource::IResourceSystem&, Archive& ar, const resource::IDevicePtr& device) const override final;

    private:
        free_type::LibraryPtr m_library;
        filesystem::IFileSystemPtr m_fileSystem;
    };
}
