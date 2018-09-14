#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>

#include "DeviceResources.h"
#include "Keng/Core/System.h"
#include "Keng/Base/Serialization/Serialization.h"
#include "Keng/FileSystem/IFileSystem.h"
#include "Keng/ResourceSystem/IResourceSystem.h"

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

    class ResourceSystem : public core::System
    <
        IResourceSystem, ResourceSystem,
        filesystem::IFileSystem
    >
    {
    public:
        ResourceSystem();
        ~ResourceSystem();

        // ISystem
        virtual void OnSystemEvent(const keng::core::IApplicationPtr&, const  keng::core::SystemEvent&) override final;

        // IResource system
        virtual IResourcePtr GetResource(const char* filename) override final;
        virtual IResourcePtr GetResource(const char* filename, const IDevicePtr& device) override final;
        virtual void AddRuntimeResource(const IResourcePtr& resource) override final;
        virtual void AddRuntimeResource(const IResourcePtr& resource, const IDevicePtr& device) override final;
        virtual void RegisterResourceFabric(const IResourceFabricPtr& fabric) override final;
        virtual void UnregisterFabric(const IResourceFabricPtr& fabric) override final;
        // ~IResource system

        IResourceFabricPtr GetFabric(const std::string& resourceType);
        DeviceResources& GetDeviceResources(const IDevicePtr& device);

        filesystem::IFileSystem& GetFileSystem();

    protected:
        SystemParams ReadDefaultParams();

    private:
    
		void Initialize(const core::IApplicationPtr& app);
		bool Update();
		void Shutdown();

    private:
        SystemParams m_parameters;

        std::vector<DeviceResourcesPtr> m_devicesResources;
        std::unordered_map<std::string, IResourceFabricPtr> m_fabrics;
    };
}
