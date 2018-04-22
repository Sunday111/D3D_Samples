#pragma once

#include <memory>

#include "Keng/Base/Serialization/Serialization.h"
#include "Keng/ResourceSystem/FwdDecl.h"

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
        virtual const char* GetResourceType() const = 0;
        virtual const char* GetNodeName() const = 0;
        virtual IResourcePtr LoadResource(IResourceSystem&, Archive& ar, const IDevicePtr& device) const = 0;
        virtual ~IResourceFabric() = default;
    };
}