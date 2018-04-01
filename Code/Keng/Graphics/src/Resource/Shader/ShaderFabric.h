#pragma once

#include "Keng/ResourceSystem/IResourceFabric.h"

namespace keng::graphics
{
    class ShaderFabric :
        public core::RefCountImpl<resource::IResourceFabric>
    {
    public:
        virtual std::string_view GetNodeName() const override;
        virtual std::string_view GetResourceType() const override;
        virtual core::Ptr<resource::IResource> LoadResource(resource::IResourceSystem*, const core::Ptr<IXmlNode>& document, const core::Ptr<resource::IDevice>& device) const override;
    };
}