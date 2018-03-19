#pragma once

#include "rapidxml.hpp"

#include <chrono>
#include <fstream>
#include <string>
#include <string_view>
#include <unordered_map>

#include "Keng/Systems/System.h"
#include "Keng/RefCounter.h"

#include "EverydayTools/UnusedVar.h"
#include "EverydayTools/Exception/CallAndRethrow.h"
#include "EverydayTools/Exception/ThrowIfFailed.h"

namespace keng
{
	class IXmlNode :
	    public IRefCountObject
	{
	public:
	    virtual IntrusivePtr<IXmlNode> FindFirstNode(std::string_view name) = 0;
		virtual IntrusivePtr<IXmlNode> NextSibling(std::string_view name = "") = 0;
	    virtual IntrusivePtr<IXmlNode> GetFirstNode(std::string_view name) = 0;
	    virtual std::string_view GetValue() const = 0;
	    virtual ~IXmlNode() = default;
	};
	
	class IXmlDocument :
	    public IXmlNode
	{
	public:
	    virtual ~IXmlDocument() = default;
	};
	
	class IResource :
	    public IRefCountObject
	{
	public:
	    virtual ~IResource() = default;
	};
	
	class IResourceSystem;
	
	class IResourceFabric :
	    public IRefCountObject
	{
	public:
	    virtual std::string_view GetResourceType() const = 0;
		virtual std::string_view GetNodeName() const = 0;
	    virtual IntrusivePtr<IResource> LoadResource(IResourceSystem*, IntrusivePtr<IXmlNode> document) const = 0;
	    virtual ~IResourceFabric() = default;
	};
	
	class IResourceSystem
	{
	public:
	    virtual IntrusivePtr<IResource> GetResource(std::string_view filename) = 0;
	    virtual ~IResourceSystem() = default;
	};
	
	template<typename Derived>
	class IXmlNodeImpl :
		public RefCountImpl<IXmlNode>
	{
	public:
		virtual IntrusivePtr<IXmlNode> FindFirstNode(std::string_view name) override {
			return CallAndRethrowM + [&]() -> IntrusivePtr<IXmlNode> {
				auto impl = GetRepresentationPtr();
				auto node = impl->first_node(name.data());
				if (!node) {
					return nullptr;
				}
				return IntrusivePtr<XmlNode>::MakeInstance(node);
			};
		}
	
		virtual IntrusivePtr<IXmlNode> GetFirstNode(std::string_view name) override {
			auto result = FindFirstNode(name);
			edt::ThrowIfFailed(result != nullptr, "Node \"", name, "\" not found");
			return result;
		}
	
		virtual IntrusivePtr<IXmlNode> NextSibling(std::string_view name) override {
			auto impl = GetRepresentationPtr();
			if (auto node = impl->next_sibling(name.data(), name.size())) {
				return IntrusivePtr<XmlNode>::MakeInstance(node);
			}
			return nullptr;
		}
	
		virtual std::string_view GetValue() const override {
			auto res = GetRepresentationPtr()->first_node();
			assert(res);
			return res->value();
		}
	
	protected:
		decltype(auto) GetRepresentationPtr() {
			auto result = static_cast<Derived&>(*this).GetHandle();
			assert(result);
			return result;
		}
	
		decltype(auto) GetRepresentationPtr() const {
			return static_cast<const Derived&>(*this).GetHandle();
		}
	};
	
	class XmlNode :
	    public IXmlNodeImpl<XmlNode>
	{
	public:
	    XmlNode(rapidxml::xml_node<char>* node) :
	        m_impl(node)
	    {
		}
	
		rapidxml::xml_node<char>* GetHandle() { return m_impl; }
		const rapidxml::xml_node<char>* GetHandle() const { return m_impl; }
	
	private:
	    rapidxml::xml_node<char>* m_impl;
	};
	
	class XmlDocument :
	    public IXmlNodeImpl<XmlDocument>
	{
	public:
	    XmlDocument(std::string_view filename) {
	        CallAndRethrowM + [&] {
	            std::ifstream ifs;
	            ifs.open(filename.data());
	            edt::ThrowIfFailed(ifs.is_open(), "Could not open file \"", filename, "\"");
	            m_content = std::string(
	                (std::istreambuf_iterator<char>(ifs)),
	                (std::istreambuf_iterator<char>()));
	            m_impl.parse<0>(&m_content[0]);
	        };
	    }
	
		rapidxml::xml_document<>* GetHandle() { return &m_impl; }
		const rapidxml::xml_document<>* GetHandle() const { return &m_impl; }
	
	private:
	    std::string m_content;
	    rapidxml::xml_document<> m_impl;
	};
	
	class ResourceSystem :
		public IResourceSystem,
		public System<ResourceSystem>
	{
	public:
		class CreateParams
		{
		public:
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
	                    if (releaseDelay + lastTouchMs < timeNow) {
	                        return true;
	                    }
	                }
	            }
	
	            return false;
	        }
	
	        IntrusivePtr<IResource> resource;
	        float releaseDelay = 0.f;
	        float lastTouchMs = -1.f;
	    };
	
	public:
		static const char* GetGUID() {
			return "8BA11029-9DE9-473C-925A-5FD0D7B36141";
		}

		virtual void Initialize(IApplication* app) override
		{
			UnusedVar(app);
			CreateParams params {};
			Initialize(params);
		}
	
		void Initialize(const CreateParams& params)
		{
			UnusedVar(params);
		}
	
	    virtual bool Update() override {
	        return CallAndRethrowM + [&] {
	            using namespace std::chrono;
	            using namespace std::chrono_literals;
	            high_resolution_clock::time_point zero(0ms);
	            auto dt = high_resolution_clock::now() - zero;
	            auto now = duration_cast<milliseconds>(dt).count();
	            float nowMs = static_cast<float>(now);
	
	            auto iResourceInfo = m_resources.begin();
	            while (iResourceInfo != m_resources.end()) {
	                if (iResourceInfo->second.ShouldBeReleased(nowMs)) {
	                    iResourceInfo = m_resources.erase(iResourceInfo);
	                } else {
	                    ++iResourceInfo;
	                }
	            }
	
	            return true;
	        };
	    }
	
	    virtual IntrusivePtr<IResource> GetResource(std::string_view filename) override {
	        return CallAndRethrowM + [&] {
	            std::string filename_copy(filename);
	            auto resource_it = m_resources.find(filename_copy);
	            if (resource_it != m_resources.end()) {
	                return resource_it->second.resource;
	            }
	
	            auto doc = IntrusivePtr<XmlDocument>::MakeInstance(filename);
	            auto resource_node = doc->GetFirstNode("resource");
	            auto typeNode = resource_node->GetFirstNode("type");
	            auto resourceTypeName = std::string(typeNode->GetValue());
	            auto fabric_it = m_fabrics.find(resourceTypeName);
	
	            edt::ThrowIfFailed(
	                fabric_it != m_fabrics.end(),
	                "Fabric for resource \"", resourceTypeName, "\" is not registered");
	
	            auto exactNode = resource_node->GetFirstNode(fabric_it->second->GetNodeName());
	            auto result = fabric_it->second->LoadResource(this, exactNode);
	
	            ResourceInfo info;
	            info.resource = result;
	            m_resources.insert(std::make_pair(std::move(filename_copy), std::move(info)));
	            return result;
	        };
	    }
	
	    void RegisterResourceFabric(IntrusivePtr<IResourceFabric> fabric) {
	        CallAndRethrowM + [&] {
	            auto resourceTypeName = fabric->GetResourceType();
	            auto res = m_fabrics.insert(std::make_pair(std::string(resourceTypeName), fabric));
	            edt::ThrowIfFailed(res.second, "Fabric \"", resourceTypeName, "\" already registered");
	        };
	    }
	
	    void UnregisterFabric(IntrusivePtr<IResourceFabric> fabric) {
	        CallAndRethrowM + [&] {
	            auto resourceTypeName = fabric->GetResourceType();
	            auto eraseCount = m_fabrics.erase(std::string(resourceTypeName));
	            edt::ThrowIfFailed(eraseCount < 1, "Fabric \"", resourceTypeName, "\" not found");
	        };
	    }
	
	public:
	protected:
	private:
	    std::unordered_map<std::string, ResourceInfo> m_resources;
	    std::unordered_map<std::string, IntrusivePtr<IResourceFabric>> m_fabrics;
	};
	
	class SimpleResourceBase :
	    public RefCountImpl<IResource>
	{
	};
}
