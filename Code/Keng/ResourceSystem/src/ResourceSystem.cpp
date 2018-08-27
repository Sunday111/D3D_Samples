#include "ResourceSystem.h"
#include "Keng/ResourceSystem/IResource.h"
#include "Keng/ResourceSystem/IResourceFabric.h"
#include "Keng/ResourceSystem/IDevice.h"
#include "Keng/Base/Serialization/OpenArchiveJSON.h"
#include "Keng/Base/Serialization/SerializeMandatory.h"
#include "Keng/FileSystem/ReadFileToBuffer.h"
#include "Keng/Core/IApplication.h"

#include "EverydayTools/Array/ArrayViewVector.h"
#include "EverydayTools/Exception/CallAndRethrow.h"
#include "EverydayTools/Exception/CheckedCast.h"
#include "EverydayTools/UnusedVar.h"

#include <algorithm>
#include <chrono>
#include <fstream>
#include "yasli/JSONOArchive.h"
#include "yasli/JSONIArchive.h"
#include "yasli/STL.h"

namespace keng::resource
{
    ResourceSystem::ResourceSystem() = default;
    
    ResourceSystem::~ResourceSystem() = default;

    void ResourceSystem::Initialize(const core::IApplicationPtr& app) {
        CallAndRethrowM + [&] {
            StoreDependencies(*app);
            m_parameters = ReadDefaultParams();
        };
    }

    bool ResourceSystem::Update() {
        return CallAndRethrowM + [&] {
            using namespace std::chrono;
            using namespace std::chrono_literals;
            high_resolution_clock::time_point zero(0ms);
            auto dt = high_resolution_clock::now() - zero;
            auto now = duration_cast<milliseconds>(dt).count();
            float nowMs = edt::CheckedCast<float>(now);

            for (auto& deviceResources : m_devicesResources) {
                deviceResources->Update(nowMs);
            }

            return true;
        };
    }

    void ResourceSystem::Shutdown() {
        m_devicesResources.clear();
        m_fabrics.clear();
    }

    IResourcePtr ResourceSystem::GetResource(const char* filename) {
        return GetResource(filename, nullptr);
    }

    void ResourceSystem::AddRuntimeResource(const IResourcePtr& resource) {
        AddRuntimeResource(resource, nullptr);
    }

    IResourcePtr ResourceSystem::GetResource(const char* filename, const IDevicePtr& device) {
        return GetDeviceResources(device).GetResource(*this, filename);
    }

    void ResourceSystem::AddRuntimeResource(const IResourcePtr& resource, const IDevicePtr& device) {
        GetDeviceResources(device).AddRuntimeResource(*this, resource);
    }

    void ResourceSystem::RegisterResourceFabric(const IResourceFabricPtr& fabric) {
        CallAndRethrowM + [&] {
            auto resourceTypeName = fabric->GetResourceType();
            auto res = m_fabrics.insert(std::make_pair(std::string(resourceTypeName), fabric));
            edt::ThrowIfFailed(res.second, "Fabric \"", resourceTypeName, "\" already registered");
        };
    }

    void ResourceSystem::UnregisterFabric(const IResourceFabricPtr& fabric) {
        CallAndRethrowM + [&] {
            auto resourceTypeName = fabric->GetResourceType();
            auto eraseCount = m_fabrics.erase(std::string(resourceTypeName));
            edt::ThrowIfFailed(eraseCount > 0, "Fabric \"", resourceTypeName, "\" not found");
        };
    }

    IResourceFabricPtr ResourceSystem::GetFabric(const std::string& resourceType) {
        return CallAndRethrowM + [&] {
            auto fabric_it = m_fabrics.find(resourceType);
            edt::ThrowIfFailed(
                fabric_it != m_fabrics.end(),
                "Fabric \"", resourceType, "\" is not registered");
            return fabric_it->second;
        };
    }

    DeviceResources& ResourceSystem::GetDeviceResources(const IDevicePtr& device) {
        auto it = std::lower_bound(m_devicesResources.begin(), m_devicesResources.end(), device,
            [](const DeviceResourcesPtr& pDeviceResources, const IDevicePtr& device) {
            return device < pDeviceResources->device;
        });

        if (it == m_devicesResources.end() || (*it)->device != device) {
            it = m_devicesResources.insert(it, std::make_unique<DeviceResources>(device));
        }

        return **it;
    }

    void SystemParams::serialize(Archive& ar) {
        ar(defaultResourceParams, "resource");
    }

    SystemParams ResourceSystem::ReadDefaultParams() {
        return CallAndRethrowM + [&] {
            struct ConfigFile {
                void serialize(Archive& ar) {
                    ar(params, "resource_system");
                }
                SystemParams params;
            };

            ConfigFile config {};

            try {
                auto filename = "Configs/resource_system.json";

                using FileView = edt::DenseArrayView<const uint8_t>;
                auto onFileRead = [&](FileView fileView) {
                    yasli::JSONIArchive ar;
                    OpenArchiveJSON(fileView, ar);
                    SerializeMandatory(ar, config);
                };
                edt::Delegate<void(FileView)> delegate;
                delegate.Bind(onFileRead);

                filesystem::HandleFileData(GetSystem<filesystem::IFileSystem>(), filename, delegate);
            } catch (...) {
            }

            return config.params;
        };
    }

    keng::filesystem::IFileSystem& ResourceSystem::GetFileSystem()
    {
        return GetSystem<filesystem::IFileSystem>();
    }

}