#include "DeviceResources.h"
#include "EverydayTools/Exception/CallAndRethrow.h"
#include "Keng/ResourceSystem/IDevice.h"
#include "Keng/ResourceSystem/IResource.h"
#include "Keng/ResourceSystem/IResourceFabric.h"
#include "ResourceSystem.h"
#include <sstream>
#include "EverydayTools/Exception/ThrowIfFailed.h"
#include "yasli/JSONIArchive.h"
#include "yasli/STL.h"
#include "Keng/Base/Serialization/SerializeMandatory.h"
#include "Keng/Base/Serialization/ReadFileToBuffer.h"
#include "Keng/Base/Serialization/OpenArchiveJSON.h"

namespace keng::resource
{
    namespace
    {
        static constexpr auto ResourceNodeName = "resource";
        static constexpr auto ResourceTypeNodeName = "type";

        struct ResourceParseInfo
        {
            ResourceParseInfo(ResourceSystem& system_, const DevicePtr& device_) :
                system(system_),
                device(device_)
            {

            }

            ResourceSystem& system;
            DevicePtr device;
            ResourcePtr resource;
            std::string type;
            void serialize(Archive& ar) {
                SerializeMandatory(ar, type, ResourceTypeNodeName);
                auto fabric = system.GetFabric(type);
                resource = fabric->LoadResource(&system, ar, device);
            }
        };

        struct FileParseInfo
        {
            FileParseInfo(ResourceSystem& system, const DevicePtr& device) :
                resourceParseInfo(system, device)
            {
            }

            ResourcePtr GetResource() const {
                return resourceParseInfo.resource;
            }

            void serialize(Archive& ar) {
                SerializeMandatory(ar, resourceParseInfo, ResourceNodeName);
            }
            ResourceParseInfo resourceParseInfo;
        };
    }

    void ResourceParameters::serialize(Archive& ar) {
        ar(releaseDelay, "release_delay");
    }

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

    DeviceResources::DeviceResources(const DevicePtr& devicePtr) :
        device(devicePtr) {

    }

    ResourcePtr DeviceResources::GetResource(ResourceSystem& system, std::string_view filename) {
        return CallAndRethrowM + [&] {
            std::string filename_copy(filename);
            auto resource_it = resources.find(filename_copy);
            if (resource_it != resources.end()) {
                return resource_it->second.resource;
            }

            auto buffer = ReadFileToBuffer(filename);

            yasli::JSONIArchive ar;
            OpenArchiveJSON(buffer, ar);

            FileParseInfo fileParseInfo(system, device);
            SerializeMandatory(ar, fileParseInfo);

            ResourceInfo info;
            info.resource = fileParseInfo.GetResource();
            return InsertResource(std::move(filename_copy), std::move(info));
        };
    }

    ResourcePtr DeviceResources::InsertResource(std::string&& name, ResourceInfo&& info) {
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

    void DeviceResources::AddRuntimeResource(ResourceSystem& system, const ResourcePtr& resource) {
        CallAndRethrowM + [&] {
            ResourceInfo info {};
            info.resource = resource;
            return InsertResource(GenerateRuntimeResourceName(), std::move(info));
        };
    }
}