#pragma once

#include "Keng/Core/ISystem.h"
#include "Keng/Core/Ptr.h"
#include "Keng/ResourceSystem/IResource.h"
#include "yasli/Archive.h"
#include <string_view>

namespace yasli
{
    class Archive;
}

namespace keng::resource
{
    class IDevice;
    class IResourceFabric;

    class IResourceSystem :
        public core::ISystem
    {
    public:
        static const char* GetGUID() { return "8BA11029-9DE9-473C-925A-5FD0D7B36141"; }

        virtual core::Ptr<IResource> GetResource(std::string_view filename) = 0;
        virtual core::Ptr<IResource> GetResource(std::string_view filename, const core::Ptr<IDevice>& deivce) = 0;
        virtual core::Ptr<IResource> MakeRuntimeResource(yasli::Archive& description) = 0;
        virtual core::Ptr<IResource> MakeRuntimeResource(yasli::Archive& description, const core::Ptr<IDevice>& deivce) = 0;
        virtual void RegisterResourceFabric(core::Ptr<IResourceFabric> fabric) = 0;
        virtual void UnregisterFabric(core::Ptr<IResourceFabric> fabric) = 0;
        virtual ~IResourceSystem() = default;
    };
}
