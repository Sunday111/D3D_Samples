#pragma once

#include <memory>
#include <string_view>

#include "Keng/Base/Serialization/Serialization.h"
#include "Keng/Core/Ptr.h"

namespace yasli
{
    class Archive;
}

namespace keng::resource
{
    class IResource;
    class IResourceSystem;
    class IDevice;

    class IResourceFabric : public core::IRefCountObject
    {
    public:
        virtual std::string_view GetResourceType() const = 0;
        virtual std::string_view GetNodeName() const = 0;
        virtual core::Ptr<IResource> LoadResource(IResourceSystem*, Archive& ar, const core::Ptr<IDevice>& device) const = 0;
        virtual ~IResourceFabric() = default;
    };
}