#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include "DeviceResources.h"
#include "Keng/ResourceSystem/IResourceSystem.h"
#include "Keng/Base/Serialization/Serialization.h"

namespace keng::resource
{
    class ResourceSystem;

    class SystemParams
    {
    public:
        void serialize(Archive& ar);

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
        virtual ResourcePtr GetResource(std::string_view filename) override;
        virtual ResourcePtr GetResource(std::string_view filename, const core::Ptr<IDevice>& device) override;
        virtual void AddRuntimeResource(const ResourcePtr& resource) override;
        virtual void AddRuntimeResource(const ResourcePtr& resource, const core::Ptr<IDevice>& device) override;
        virtual void RegisterResourceFabric(const ResourceFabricPtr& fabric) override;
        virtual void UnregisterFabric(const ResourceFabricPtr& fabric) override;
        ResourceFabricPtr GetFabric(const std::string& resourceType);
        DeviceResources& GetDeviceResources(const core::Ptr<IDevice>& device);

    protected:
        SystemParams ReadDefaultParams();

    protected:
        std::vector<std::string> m_dependencies;

    private:
        SystemParams m_parameters;

        std::vector<DeviceResourcesPtr> m_devicesResources;
        std::unordered_map<std::string, ResourceFabricPtr> m_fabrics;
    };
}
