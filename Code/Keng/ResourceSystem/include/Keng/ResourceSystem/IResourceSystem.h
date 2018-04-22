#pragma once

#include "Keng/Core/ISystem.h"
#include "Keng/FileSystem/FwdDecl.h"
#include "Keng/ResourceSystem/FwdDecl.h"
#include "Keng/ResourceSystem/IResource.h"

namespace keng::resource
{
    class IResourceSystem : public core::ISystem
    {
    public:
        static const char* SystemName() { return "KengResourceSystem"; }
        virtual IResourcePtr GetResource(const char* filename) = 0;
        virtual IResourcePtr GetResource(const char* filename, const IDevicePtr& deivce) = 0;
        virtual void AddRuntimeResource(const IResourcePtr& resource) = 0;
        virtual void AddRuntimeResource(const IResourcePtr& resource, const IDevicePtr& deivce) = 0;
        virtual void RegisterResourceFabric(const IResourceFabricPtr& fabric) = 0;
        virtual void UnregisterFabric(const IResourceFabricPtr& fabric) = 0;
        virtual ~IResourceSystem() = default;
    };
}
