#pragma once

#include "FwdDecl.h"
#include "Keng/ResourceSystem/IResourceFabric.h"

namespace keng::graphics
{
    class TextureFabric :
        public core::RefCountImpl<resource::IResourceFabric>
    {
    public:
        virtual std::string_view GetNodeName() const override;
        virtual std::string_view GetResourceType() const override;
        virtual resource::IResourcePtr LoadResource(resource::IResourceSystem*, Archive& ar, const resource::IDevicePtr& device) const override;
    };
}
