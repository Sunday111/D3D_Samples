#pragma once

#include <string>
#include <string_view>
#include <unordered_map>
#include "Keng/ResourceSystem/IResourceSystem.h"

namespace keng::resource
{
    class ResourceSystem :
        public IResourceSystem
    {
    public:
        struct ResourceParameters
        {
            float releaseDelay = 0.0f;
        };

        struct SystemParams
        {
            ResourceParameters defaultResourceParams;
        };

    protected:
    private:
        struct ResourceInfo
        {
            ResourceInfo();
            ~ResourceInfo();

            bool IsSingleReference() const;
            bool IsExpired() const;
            bool ShouldBeReleased(float timeNow);

            ResourceParameters params;
            core::Ptr<IResource> resource;
            float lastTouchMs = -1.f;
        };

    public:
        ResourceSystem();
        ~ResourceSystem();

        // ISystem
        virtual bool ForEachSystemDependency(bool(*pfn)(const char* systemGUID, void* context), void* context) override;
        virtual const char* GetSystemGUID() override;
        virtual void Initialize(core::IApplication* app) override;
        virtual bool Update() override;

        // IResource system
        virtual core::Ptr<IResource> GetResource(std::string_view filename) override;
        virtual core::Ptr<IResource> MakeRuntimeResource(core::Ptr<IXmlDocument> description) override;
        virtual void RegisterResourceFabric(core::Ptr<IResourceFabric> fabric) override;
        virtual void UnregisterFabric(core::Ptr<IResourceFabric> fabric) override;

    protected:
        SystemParams ReadDefaultParams();
        core::Ptr<IResource> InsertResource(std::string&& name, ResourceInfo&& info);
        std::string GenerateRuntimeResourceName();

    protected:
        std::vector<std::string> m_dependencies;

    private:
        size_t m_nextRuntimeResourceIndex = 0;
        static constexpr auto ResourceNodeName = "resource";
        static constexpr auto ResourceTypeNodeName = "type";
        SystemParams m_parameters;
        std::unordered_map<std::string, ResourceInfo> m_resources;
        std::unordered_map<std::string, core::Ptr<IResourceFabric>> m_fabrics;
    };
}
