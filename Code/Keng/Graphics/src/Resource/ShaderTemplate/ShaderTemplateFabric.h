#pragma once

#include "Keng/ResourceSystem/IResourceFabric.h"

namespace keng::graphics
{
    class ShaderTemplateFabric :
        public core::RefCountImpl<resource::IResourceFabric>
    {
    public:
        virtual std::string_view GetNodeName() const override;
        virtual std::string_view GetResourceType() const override;
        virtual core::Ptr<resource::IResource> LoadResource(resource::IResourceSystem*, Archive& ar, const core::Ptr<resource::IDevice>& device) const override;
    };
}
