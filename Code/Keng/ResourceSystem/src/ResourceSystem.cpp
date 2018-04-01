#include "ResourceSystem.h"
#include "Keng/ResourceSystem/IResource.h"
#include "Keng/ResourceSystem/IResourceFabric.h"

#include "EverydayTools/Exception/CallAndRethrow.h"
#include "EverydayTools/Exception/ThrowIfFailed.h"
#include "EverydayTools/UnusedVar.h"

#include "Xml.h"

#include <algorithm>
#include <chrono>
#include <fstream>

namespace keng::resource
{
    ResourceInfo::ResourceInfo() = default;
    ResourceInfo::~ResourceInfo() = default;

    ResourceSystem::ResourceSystem() = default;

    ResourceSystem::~ResourceSystem() = default;

    bool ResourceInfo::IsSingleReference() const {
        return resource->GetReferencesCount() < 2;
    }

    bool ResourceInfo::IsExpired() const {
        return false;
    }

    bool ResourceInfo::ShouldBeReleased(float timeNow) {
        if (IsSingleReference()) {
            if (lastTouchMs < 0.f) {
                // Start countdown before dying
                lastTouchMs = timeNow;
            } else {
                if (params.releaseDelay + lastTouchMs < timeNow) {
                    return true;
                }
            }
        }

        return false;
    }

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

    DeviceResources::DeviceResources(const core::Ptr<IDevice>& devicePtr) :
        device(devicePtr)
    {

    }

    core::Ptr<IResource> DeviceResources::GetResource(ResourceSystem& system, std::string_view filename) {
        return CallAndRethrowM + [&] {
            std::string filename_copy(filename);
            auto resource_it = resources.find(filename_copy);
            if (resource_it != resources.end()) {
                return resource_it->second.resource;
            }

            auto doc = std::make_shared<XmlDocument>(filename);
            auto resource_node = doc->GetFirstNode(ResourceNodeName);
            auto typeNode = resource_node->GetFirstNode(ResourceTypeNodeName);
            auto resourceTypeName = std::string(typeNode->GetValue());
            auto fabric = system.GetFabric(resourceTypeName);
            auto exactNode = resource_node->GetFirstNode(fabric->GetNodeName());
            auto result = fabric->LoadResource(&system, exactNode, device);

            ResourceInfo info;
            info.resource = result;
            return InsertResource(std::move(filename_copy), std::move(info));
        };
    }

    core::Ptr<IResource> ResourceSystem::GetResource(std::string_view filename) {
        return GetResource(filename, nullptr);
    }

    core::Ptr<IResource> ResourceSystem::MakeRuntimeResource(core::Ptr<IXmlDocument> description) {
        return MakeRuntimeResource(description, nullptr);
    }

    core::Ptr<IResource> ResourceSystem::GetResource(std::string_view filename, const core::Ptr<IDevice>& device) {
        return GetDeviceResources(device).GetResource(*this, filename);
    }

    core::Ptr<IResource> ResourceSystem::MakeRuntimeResource(core::Ptr<IXmlDocument> description, const core::Ptr<IDevice>& device) {
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

    SystemParams ResourceSystem::ReadDefaultParams() {
        return CallAndRethrowM + [&] {
            SystemParams result;

            try {
                XmlDocument configDoc("Configs/resource_system.xml");
                auto resourceSystemNode = configDoc.GetFirstNode("resource_system");
                if (auto resourceNode = resourceSystemNode->FindFirstNode("resource")) {
                    if (auto releaseDelayNode = resourceNode->FindFirstNode("release_delay")) {
                        auto releaseDelayText = releaseDelayNode->GetValue();
                        result.defaultResourceParams.releaseDelay = std::stof(releaseDelayText.data());
                    }
                }
            } catch (...) {
            }

            return result;
        };
    }

    core::Ptr<IResource> DeviceResources::InsertResource(std::string&& name, ResourceInfo&& info) {
        auto resource = info.resource;
        resources.insert(std::make_pair(std::move(name), std::move(info)));
        return resource;
    }

    void DeviceResources::Update(float currentTime) {
        auto iResourceInfo = resources.begin();
        while (iResourceInfo != resources.end()) {
            if (iResourceInfo->second.ShouldBeReleased(currentTime)) {
                iResourceInfo = resources.erase(iResourceInfo);
            } else {
                ++iResourceInfo;
            }
        }
    }

    std::string DeviceResources::GenerateRuntimeResourceName() {
        std::stringstream stream;
        stream << "runtime://" << m_nextRuntimeResourceIndex++;
        return stream.str();
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

    core::Ptr<IResource> DeviceResources::MakeRuntimeResource(ResourceSystem& system, core::Ptr<IXmlDocument> doc) {
        return CallAndRethrowM + [&] {
            auto resource_node = doc->GetFirstNode(ResourceNodeName);
            auto typeNode = resource_node->GetFirstNode(ResourceTypeNodeName);
            auto resourceTypeName = std::string(typeNode->GetValue());
            auto fabric = system.GetFabric(resourceTypeName);

            auto exactNode = resource_node->GetFirstNode(fabric->GetNodeName());
            auto result = fabric->LoadResource(&system, exactNode, device);

            ResourceInfo info;
            info.resource = result;
            return InsertResource(GenerateRuntimeResourceName(), std::move(info));
        };
    }
}