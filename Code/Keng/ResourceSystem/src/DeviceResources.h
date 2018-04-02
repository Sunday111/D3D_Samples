#pragma once

#include <string>
#include <string_view>
#include <unordered_map>

#include "Keng/Base/Serialization/Serialization.h"
#include "Keng/Core/IRefCountObject.h"
#include "Keng/Core/Ptr.h"

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
        core::Ptr<IResource> resource;
        float lastTouchMs = -1.f;
    };

    class DeviceResources
    {
    public:
        DeviceResources(const core::Ptr<IDevice>& devicePtr);

        core::Ptr<IResource> GetResource(ResourceSystem& system, std::string_view filename);
        core::Ptr<IResource> InsertResource(std::string&& name, ResourceInfo&& info);
        void AddRuntimeResource(ResourceSystem& system, const core::Ptr<IResource>& resource);
        void Update(float currentTime);
        std::string GenerateRuntimeResourceName();

        core::Ptr<IDevice> device;
        std::unordered_map<std::string, ResourceInfo> resources;
        size_t m_nextRuntimeResourceIndex = 0;
    };
}
