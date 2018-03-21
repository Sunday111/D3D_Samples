#pragma once

#include <memory>
#include <string_view>

namespace keng
{
    class IXmlNode;
}

namespace keng::resource
{
    class IResource;
    class IResourceSystem;

    class IResourceFabric
    {
    public:
        virtual std::string_view GetResourceType() const = 0;
        virtual std::string_view GetNodeName() const = 0;
        virtual std::shared_ptr<IResource> LoadResource(IResourceSystem*, std::shared_ptr<IXmlNode> document) const = 0;
        virtual ~IResourceFabric() = default;
    };
}