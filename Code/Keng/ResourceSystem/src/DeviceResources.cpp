#include "DeviceResources.h"
#include "EverydayTools/Exception/CallAndRethrow.h"
#include "Keng/ResourceSystem/IDevice.h"
#include "Keng/ResourceSystem/IResource.h"
#include "Keng/ResourceSystem/IResourceFabric.h"
#include "xml.h"
#include "ResourceSystem.h"

namespace keng::resource
{
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

    DeviceResources::DeviceResources(const core::Ptr<IDevice>& devicePtr) :
        device(devicePtr) {

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