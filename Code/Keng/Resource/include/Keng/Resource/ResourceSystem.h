#pragma once

#include "Keng/Core/RefCounter.h"
#include "Keng/Core/Systems/ISystem.h"
#include "Xml.h"

#include <string_view>

namespace keng
{
    class IResource :
        public IRefCountObject
    {
    public:
        virtual ~IResource() = default;
    };

    class IResourceSystem;

    class IResourceFabric :
        public IRefCountObject
    {
    public:
        virtual std::string_view GetResourceType() const = 0;
        virtual std::string_view GetNodeName() const = 0;
        virtual IntrusivePtr<IResource> LoadResource(IResourceSystem*, IntrusivePtr<IXmlNode> document) const = 0;
        virtual ~IResourceFabric() = default;
    };

    class IResourceSystem :
        public ISystem
    {
    public:
        static const char* GetGUID() { return "8BA11029-9DE9-473C-925A-5FD0D7B36141"; }

        virtual IntrusivePtr<IResource> GetResource(std::string_view filename) = 0;
        virtual void RegisterResourceFabric(IntrusivePtr<IResourceFabric> fabric) = 0;
        virtual void UnregisterFabric(IntrusivePtr<IResourceFabric> fabric) = 0;
        virtual ~IResourceSystem() = default;
    };

    IResourceSystem* CreateResourceSystem();
}
