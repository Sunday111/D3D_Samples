#pragma once

#include <string>
#include <string_view>
#include <unordered_map>
#include "Keng/ResourceSystem/IResourceSystem.h"
#include "Keng//ResourceSystem/IDevice.h"

namespace keng::resource
{
    class ResourceSystem;

    class ResourceParameters
    {
    public:
        float releaseDelay = 0.0f;
    };

    class SystemParams
    {
    public:
        ResourceParameters defaultResourceParams;
    };

    class ResourceInfo
    {
    public:
        ResourceInfo();
        ~ResourceInfo();

        bool IsSingleReference() const;
        bool IsExpired() const;
        bool ShouldBeReleased(float timeNow);

        ResourceParameters params;
        core::Ptr<IResource> resource;
        float lastTouchMs = -1.f;
    };

    class DeviceResources
    {
    public:
        DeviceResources(const core::Ptr<IDevice>& devicePtr);

        core::Ptr<IResource> GetResource(ResourceSystem& system, std::string_view filename);
        core::Ptr<IResource> InsertResource(std::string&& name, ResourceInfo&& info);
        core::Ptr<IResource> MakeRuntimeResource(ResourceSystem& system, core::Ptr<IXmlDocument> description);
        void Update(float currentTime);
        std::string GenerateRuntimeResourceName();

        core::Ptr<IDevice> device;
        std::unordered_map<std::string, ResourceInfo> resources;
        static constexpr auto ResourceNodeName = "resource";
        static constexpr auto ResourceTypeNodeName = "type";
        size_t m_nextRuntimeResourceIndex = 0;
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
        virtual core::Ptr<IResource> MakeRuntimeResource(core::Ptr<IXmlDocument> description) override;
        virtual core::Ptr<IResource> MakeRuntimeResource(core::Ptr<IXmlDocument> description, const core::Ptr<IDevice>& device) override;
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
