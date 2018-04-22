#include "DeviceResources.h"
#include "EverydayTools/Exception/CallAndRethrow.h"
#include "EverydayTools/Array/ArrayViewVector.h"
#include "Keng/ResourceSystem/IDevice.h"
#include "Keng/ResourceSystem/IResource.h"
#include "Keng/ResourceSystem/IResourceFabric.h"
#include "ResourceSystem.h"
#include <sstream>
#include "EverydayTools/Exception/ThrowIfFailed.h"
#include "yasli/JSONIArchive.h"
#include "yasli/STL.h"
#include "Keng/Base/Serialization/SerializeMandatory.h"
#include "Keng/Base/Serialization/OpenArchiveJSON.h"
#include "Keng/FileSystem/ReadFileToBuffer.h"

namespace keng::resource
{
    namespace
    {
        static constexpr auto ResourceNodeName = "resource";
        static constexpr auto ResourceTypeNodeName = "type";

        struct ResourceParseInfo
        {
            ResourceParseInfo(ResourceSystem& system_, const IDevicePtr& device_) :
                system(system_),
                device(device_)
            {

            }

            void serialize(Archive& ar) {
                SerializeMandatory(ar, type, ResourceTypeNodeName);
                auto fabric = system.GetFabric(type);
                resource = fabric->LoadResource(system, ar, device);
            }

            ResourceSystem& system;
            IDevicePtr device;
            IResourcePtr resource;
            std::string type;
        };

        struct FileParseInfo
        {
            FileParseInfo(ResourceSystem& system, const IDevicePtr& device) :
                resourceParseInfo(system, device)
            {
            }

            IResourcePtr GetResource() const {
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

    DeviceResources::DeviceResources(const IDevicePtr& devicePtr) :
        device(devicePtr) {

    }

    IResourcePtr DeviceResources::GetResource(ResourceSystem& system, std::string_view filename) {
        return CallAndRethrowM + [&] {
            std::string filename_copy(filename);
            auto resource_it = resources.find(filename_copy);
            if (resource_it != resources.end()) {
                return resource_it->second.resource;
            }

            using FileView = edt::DenseArrayView<const uint8_t>;
            IResourcePtr result;
            auto onFileRead = [&](FileView fileView) {

                yasli::JSONIArchive ar;
                OpenArchiveJSON(fileView, ar);

                FileParseInfo fileParseInfo(system, device);
                SerializeMandatory(ar, fileParseInfo);

                ResourceInfo info;
                info.resource = fileParseInfo.GetResource();
                result = InsertResource(std::move(filename_copy), std::move(info));
            };
            edt::Delegate<void(FileView)> delegate;
            delegate.Bind(onFileRead);
            filesystem::HandleFileData(filename.data(), delegate);
            return result;
        };
    }

    IResourcePtr DeviceResources::InsertResource(std::string&& name, ResourceInfo&& info) {
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

    void DeviceResources::AddRuntimeResource(ResourceSystem& system, const IResourcePtr& resource) {
        CallAndRethrowM + [&] {
            ResourceInfo info {};
            info.resource = resource;
            return InsertResource(GenerateRuntimeResourceName(), std::move(info));
        };
    }
}