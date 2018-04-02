#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include "DeviceResources.h"
#include "Keng/ResourceSystem/IResourceSystem.h"

namespace keng::resource
{
    class ResourceSystem;

    class SystemParams
    {
    public:
        void serialize(yasli::Archive& ar);

        ResourceParameters defaultResourceParams;
    };

    using DeviceResourcesPtr = std::unique_ptr<DeviceResources>;

    class ResourceSystem :
        public IResourceSystem
    {
    public:
        ResourceSystem();
        ~ResourceSystem();

        // ISystem
        virtual bool ForEachSystemDependency(bool(*pfn)(const char* systemGUID, void* context), void* context) override;
        virtual const char* GetSystemGUID() override;
        virtual void Initialize(core::IApplication* app) override;
        virtual bool Update() override;

        // IResource system
        virtual core::Ptr<IResource> GetResource(std::string_view filename) override;
        virtual core::Ptr<IResource> GetResource(std::string_view filename, const core::Ptr<IDevice>& device) override;
        virtual core::Ptr<IResource> MakeRuntimeResource(yasli::Archive& description) override;
        virtual core::Ptr<IResource> MakeRuntimeResource(yasli::Archive& description, const core::Ptr<IDevice>& device) override;
        virtual void RegisterResourceFabric(core::Ptr<IResourceFabric> fabric) override;
        virtual void UnregisterFabric(core::Ptr<IResourceFabric> fabric) override;
        core::Ptr<IResourceFabric> GetFabric(const std::string& resourceType);
        DeviceResources& GetDeviceResources(const core::Ptr<IDevice>& device);

    protected:
        SystemParams ReadDefaultParams();

    protected:
        std::vector<std::string> m_dependencies;

    private:
        SystemParams m_parameters;

        std::vector<DeviceResourcesPtr> m_devicesResources;
        std::unordered_map<std::string, core::Ptr<IResourceFabric>> m_fabrics;
    };
}
