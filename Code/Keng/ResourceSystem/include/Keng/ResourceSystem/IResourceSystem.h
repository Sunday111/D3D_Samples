#pragma once

#include "Keng/ResourceSystem/FwdDecl.h"
#include "Keng/Base/Serialization/Serialization.h"
#include "Keng/Core/ISystem.h"
#include "Keng/ResourceSystem/IResource.h"
#include "yasli/Archive.h"
#include <string_view>

namespace keng::resource
{
    class IDevice;
    class IResourceFabric;

    class IResourceSystem :
        public core::ISystem
    {
    public:
        static std::string_view SystemName() { return "KengResourceSystem"; }
        virtual IResourcePtr GetResource(std::string_view filename) = 0;
        virtual IResourcePtr GetResource(std::string_view filename, const IDevicePtr& deivce) = 0;
        virtual void AddRuntimeResource(const IResourcePtr& resource) = 0;
        virtual void AddRuntimeResource(const IResourcePtr& resource, const IDevicePtr& deivce) = 0;
        virtual void RegisterResourceFabric(const IResourceFabricPtr& fabric) = 0;
        virtual void UnregisterFabric(const IResourceFabricPtr& fabric) = 0;
        virtual ~IResourceSystem() = default;
    };
}
