#pragma once

#include "Keng/Core/ISystem.h"
#include "Keng/FileSystem/FwdDecl.h"
#include "Keng/ResourceSystem/FwdDecl.h"
#include "Keng/ResourceSystem/IResource.h"
#include <string_view>

namespace keng::resource
{
    class IResourceSystem : public core::ISystem
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
