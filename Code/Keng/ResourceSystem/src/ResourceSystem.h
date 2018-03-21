#pragma once

#include "Keng/ResourceSystem/IResourceSystem.h"

#include "Xml.h"

#include <chrono>
#include <fstream>
#include <string>
#include <string_view>
#include <unordered_map>

#include "EverydayTools/UnusedVar.h"
#include "EverydayTools/Exception/CallAndRethrow.h"
#include "EverydayTools/Exception/ThrowIfFailed.h"

namespace keng
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
            bool IsSingleReference() const;
            bool IsExpired() const;
            bool ShouldBeReleased(float timeNow);
	
            ResourceParameters params;
            std::shared_ptr<IResource> resource;
	        float lastTouchMs = -1.f;
	    };
	
	public:
        // ISystem
        virtual bool ForEachSystemDependency(bool(*pfn)(const char* systemGUID, void* context), void* context) override;
        virtual const char* GetSystemGUID() override;
        virtual void Initialize(IApplication* app) override;
        virtual bool Update() override;

        // IResource system
        virtual std::shared_ptr<IResource> GetResource(std::string_view filename) override;
        virtual void RegisterResourceFabric(std::shared_ptr<IResourceFabric> fabric) override;
        virtual void UnregisterFabric(std::shared_ptr<IResourceFabric> fabric) override;

    protected:
        SystemParams ReadDefaultParams();
	
	public:
	protected:
        std::vector<std::string> m_dependencies;

	private:
        SystemParams m_parameters;
	    std::unordered_map<std::string, ResourceInfo> m_resources;
	    std::unordered_map<std::string, std::shared_ptr<IResourceFabric>> m_fabrics;
	};
}
