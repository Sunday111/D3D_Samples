#pragma once

#include "Keng/ResourceSystem/IResourceFabric.h"

namespace keng::graphics
{
    class Device;

    class ShaderFabric :
        public core::RefCountImpl<resource::IResourceFabric>
    {
    public:
        ShaderFabric(core::Ptr<Device>);

        virtual std::string_view GetNodeName() const override;
        virtual std::string_view GetResourceType() const override;
        virtual core::Ptr<resource::IResource> LoadResource(resource::IResourceSystem*, core::Ptr<IXmlNode> document) const override;

    private:
        core::Ptr<Device> m_device;
    };
}