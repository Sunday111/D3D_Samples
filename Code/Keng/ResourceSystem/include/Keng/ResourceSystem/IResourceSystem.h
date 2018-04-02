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
        static const char* GetGUID() { return "8BA11029-9DE9-473C-925A-5FD0D7B36141"; }

        virtual ResourcePtr GetResource(std::string_view filename) = 0;
        virtual ResourcePtr GetResource(std::string_view filename, const DevicePtr& deivce) = 0;
        virtual void AddRuntimeResource(const ResourcePtr& resource) = 0;
        virtual void AddRuntimeResource(const ResourcePtr& resource, const DevicePtr& deivce) = 0;
        virtual void RegisterResourceFabric(const ResourceFabricPtr& fabric) = 0;
        virtual void UnregisterFabric(const ResourceFabricPtr& fabric) = 0;
        virtual ~IResourceSystem() = default;
    };
}
