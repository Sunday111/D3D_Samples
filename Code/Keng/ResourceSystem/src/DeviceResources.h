#pragma once

#include <string>
#include <string_view>
#include <unordered_map>

#include "Keng/ResourceSystem/FwdDecl.h"
#include "Keng/Base/Serialization/Serialization.h"
#include "Keng/Core/IRefCountObject.h"

namespace yasli
{
    class Archive;
}

namespace keng::resource
{
    class IDevice;
    class IResource;
    class ResourceSystem;

    class ResourceParameters
    {
    public:
        void serialize(Archive& ar);

        float releaseDelay = 0.0f;
    };

    class ResourceInfo
    {
    public:
        bool IsSingleReference() const;
        bool IsExpired() const;
        bool ShouldBeReleased(float timeNow);

        ResourceParameters params;
        IResourcePtr resource;
        float lastTouchMs = -1.f;
    };

    class DeviceResources
    {
    public:
        DeviceResources(const IDevicePtr& devicePtr);

        IResourcePtr GetResource(ResourceSystem& system, std::string_view filename);
        IResourcePtr InsertResource(std::string&& name, ResourceInfo&& info);
        void AddRuntimeResource(ResourceSystem& system, const IResourcePtr& resource);
        void Update(float currentTime);
        std::string GenerateRuntimeResourceName();

        IDevicePtr device;
        std::unordered_map<std::string, ResourceInfo> resources;
        size_t m_nextRuntimeResourceIndex = 0;
    };
}
