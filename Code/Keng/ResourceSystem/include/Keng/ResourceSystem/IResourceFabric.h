#pragma once

#include <memory>
#include <string_view>

#include "Keng/Core/Ptr.h"

namespace keng
{
    class IXmlNode;
}

namespace keng::resource
{
    class IResource;
    class IResourceSystem;

    class IResourceFabric : public core::IRefCountObject
    {
    public:
        virtual std::string_view GetResourceType() const = 0;
        virtual std::string_view GetNodeName() const = 0;
        virtual core::Ptr<IResource> LoadResource(IResourceSystem*, core::Ptr<IXmlNode> document) const = 0;
        virtual ~IResourceFabric() = default;
    };
}