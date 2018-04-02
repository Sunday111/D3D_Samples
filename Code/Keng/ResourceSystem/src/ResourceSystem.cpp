#include "ResourceSystem.h"
#include "Keng/ResourceSystem/IResource.h"
#include "Keng/ResourceSystem/IResourceFabric.h"
#include "Keng/ResourceSystem/IDevice.h"
#include "keng/ResourceSystem/OpenArchiveJSON.h"

#include "EverydayTools/Exception/CallAndRethrow.h"
#include "EverydayTools/Exception/ThrowIfFailed.h"
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

    void ResourceSystem::Initialize(core::IApplication* app) {
        UnusedVar(app);
        m_parameters = ReadDefaultParams();
    }

    bool ResourceSystem::Update() {
        return CallAndRethrowM + [&] {
            using namespace std::chrono;
            using namespace std::chrono_literals;
            high_resolution_clock::time_point zero(0ms);
            auto dt = high_resolution_clock::now() - zero;
            auto now = duration_cast<milliseconds>(dt).count();
            float nowMs = static_cast<float>(now);

            for (auto& deviceResources : m_devicesResources) {
                deviceResources->Update(nowMs);
            }

            return true;
        };
    }

    core::Ptr<IResource> ResourceSystem::GetResource(std::string_view filename) {
        return GetResource(filename, nullptr);
    }

    core::Ptr<IResource> ResourceSystem::MakeRuntimeResource(yasli::Archive& description) {
        return MakeRuntimeResource(description, nullptr);
    }

    core::Ptr<IResource> ResourceSystem::GetResource(std::string_view filename, const core::Ptr<IDevice>& device) {
        return GetDeviceResources(device).GetResource(*this, filename);
    }

    core::Ptr<IResource> ResourceSystem::MakeRuntimeResource(yasli::Archive& description, const core::Ptr<IDevice>& device) {
        return GetDeviceResources(device).MakeRuntimeResource(*this, description);
    }

    void ResourceSystem::RegisterResourceFabric(core::Ptr<IResourceFabric> fabric) {
        CallAndRethrowM + [&] {
            auto resourceTypeName = fabric->GetResourceType();
            auto res = m_fabrics.insert(std::make_pair(std::string(resourceTypeName), fabric));
            edt::ThrowIfFailed(res.second, "Fabric \"", resourceTypeName, "\" already registered");
        };
    }

    void ResourceSystem::UnregisterFabric(core::Ptr<IResourceFabric> fabric) {
        CallAndRethrowM + [&] {
            auto resourceTypeName = fabric->GetResourceType();
            auto eraseCount = m_fabrics.erase(std::string(resourceTypeName));
            edt::ThrowIfFailed(eraseCount < 1, "Fabric \"", resourceTypeName, "\" not found");
        };
    }

    core::Ptr<IResourceFabric> ResourceSystem::GetFabric(const std::string& resourceType) {
        return CallAndRethrowM + [&] {
            auto fabric_it = m_fabrics.find(resourceType);
            edt::ThrowIfFailed(
                fabric_it != m_fabrics.end(),
                "Fabric \"", resourceType, "\" is not registered");
            return fabric_it->second;
        };
    }

    DeviceResources& ResourceSystem::GetDeviceResources(const core::Ptr<IDevice>& device) {
        auto it = std::lower_bound(m_devicesResources.begin(), m_devicesResources.end(), device,
            [](const DeviceResourcesPtr& pDeviceResources, const core::Ptr<IDevice>& device) {
            return device < pDeviceResources->device;
        });

        if (it == m_devicesResources.end() || (*it)->device != device) {
            it = m_devicesResources.insert(it, std::make_unique<DeviceResources>(device));
        }

        return **it;
    }

    void SystemParams::serialize(yasli::Archive& ar) {
        ar(defaultResourceParams, "resource");
    }

    SystemParams ResourceSystem::ReadDefaultParams() {
        return CallAndRethrowM + [&] {
            struct ConfigFile {
                void serialize(yasli::Archive& ar) {
                    ar(params, "resource_system");
                }
                SystemParams params;
            };

            ConfigFile file {};

            try {
                auto buffer = ReadFileToBuffer("Configs/resource_system.json");

                yasli::JSONIArchive ar;
                OpenArchiveJSON(buffer, ar);
                SerializeMandatory(ar, file);
            } catch (...) {
            }

            return file.params;
        };
    }

    bool ResourceSystem::ForEachSystemDependency(bool(*pfn)(const char* systemGUID, void* context), void* context) {
        return CallAndRethrowM + [&]() -> bool {
            for (auto& guid : m_dependencies) {
                if (pfn(guid.data(), context)) {
                    return true;
                }
            }

            return false;
        };
    }

    const char* ResourceSystem::GetSystemGUID() {
        return GetGUID();
    }
}