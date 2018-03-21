#pragma once

#include "Xml.h"

#include <chrono>
#include <fstream>
#include <string>
#include <string_view>
#include <unordered_map>

#include "Keng/Core/Systems/System.h"
#include "Keng/Core/RefCounter.h"

#include "EverydayTools/UnusedVar.h"
#include "EverydayTools/Exception/CallAndRethrow.h"
#include "EverydayTools/Exception/ThrowIfFailed.h"

namespace keng
{
	class ResourceSystem :
		public IResourceSystem,
		public System<ResourceSystem>
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
	        bool IsSingleReference() const {
	            return RefCounter::GetReferencesCount(resource.Get()) < 2;
	        }
	
	        bool IsExpired() const {
	            return false;
	        }
	
	        bool ShouldBeReleased(float timeNow) {
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
	
            ResourceParameters params;
	        IntrusivePtr<IResource> resource;
	        float lastTouchMs = -1.f;
	    };
	
	public:
        static const char* GetGUID();
        virtual void Initialize(IApplication* app) override;	
        virtual bool Update() override;	
        virtual IntrusivePtr<IResource> GetResource(std::string_view filename) override;	
        void RegisterResourceFabric(IntrusivePtr<IResourceFabric> fabric);
        void UnregisterFabric(IntrusivePtr<IResourceFabric> fabric);

    protected:
        SystemParams ReadDefaultParams();
	
	public:
	protected:
	private:
        SystemParams m_parameters;
	    std::unordered_map<std::string, ResourceInfo> m_resources;
	    std::unordered_map<std::string, IntrusivePtr<IResourceFabric>> m_fabrics;
	};
	
	class SimpleResourceBase :
	    public RefCountImpl<IResource>
	{
	};
}
