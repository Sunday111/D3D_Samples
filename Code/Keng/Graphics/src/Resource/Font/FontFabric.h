#pragma once

#include "FwdDecl.h"
#include "Resource/Font/FreeType/FwdDecl.h"
#include "Keng/ResourceSystem/IResourceFabric.h"

namespace keng::graphics
{
    class FontFabric :
        public core::RefCountImpl<resource::IResourceFabric>
    {
    public:
        FontFabric();
        ~FontFabric();

        virtual std::string_view GetNodeName() const override;
        virtual std::string_view GetResourceType() const override;
        virtual resource::IResourcePtr LoadResource(resource::IResourceSystem&, Archive& ar, const resource::IDevicePtr& device) const override;

    private:
        free_type::LibraryPtr m_library;
    };
}
