#pragma once

#include "Keng/Core/Systems/ISystem.h"
#include "Xml.h"

#include <string_view>

namespace keng::resource
{
    class IResource;
    class IResourceFabric;

    class IResourceSystem :
        public core::ISystem
    {
    public:
        static const char* GetGUID() { return "8BA11029-9DE9-473C-925A-5FD0D7B36141"; }

        virtual std::shared_ptr<IResource> GetResource(std::string_view filename) = 0;
        virtual void RegisterResourceFabric(std::shared_ptr<IResourceFabric> fabric) = 0;
        virtual void UnregisterFabric(std::shared_ptr<IResourceFabric> fabric) = 0;
        virtual ~IResourceSystem() = default;
    };

    __declspec(dllexport) IResourceSystem* CreateResourceSystem();
}
