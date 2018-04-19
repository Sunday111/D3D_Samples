#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include "DeviceResources.h"
#include "Keng/ResourceSystem/IResourceSystem.h"
#include "Keng/Base/Serialization/Serialization.h"
#include "Keng/FileSystem/FwdDecl.h"

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

    class ResourceSystem : public core::RefCountImpl<IResourceSystem>
    {
    public:
        ResourceSystem();
        ~ResourceSystem();

        // ISystem
        virtual std::string_view GetSystemName() const override;
        virtual bool ForEachDependency(const edt::Delegate<bool(std::string_view systemName)>& delegate) const override;
        virtual void Initialize(const core::IApplicationPtr& app) override;
        virtual bool Update() override;
        virtual void Shutdown() override;

        // IResource system
        virtual IResourcePtr GetResource(std::string_view filename) override;
        virtual IResourcePtr GetResource(std::string_view filename, const IDevicePtr& device) override;
        virtual void AddRuntimeResource(const IResourcePtr& resource) override;
        virtual void AddRuntimeResource(const IResourcePtr& resource, const IDevicePtr& device) override;
        virtual void RegisterResourceFabric(const IResourceFabricPtr& fabric) override;
        virtual void UnregisterFabric(const IResourceFabricPtr& fabric) override;

        IResourceFabricPtr GetFabric(const std::string& resourceType);
        DeviceResources& GetDeviceResources(const IDevicePtr& device);

    protected:
        SystemParams ReadDefaultParams();

    private:
        SystemParams m_parameters;

        std::vector<DeviceResourcesPtr> m_devicesResources;
        std::unordered_map<std::string, IResourceFabricPtr> m_fabrics;
        filesystem::IFileSystemPtr m_filesystem;
    };
}
