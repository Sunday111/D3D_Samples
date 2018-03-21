#pragma once

#include "rapidxml.hpp"
#include "EverydayTools/Exception/CallAndRethrow.h"
#include "EverydayTools/Exception/ThrowIfFailed.h"
#include "Keng/Core/RefCounter.h"
#include <fstream>
#include <string_view>

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

        rapidxml::xml_document<>* XmlDocument::GetHandle() {
            return &m_impl;
        }

        const rapidxml::xml_document<>* XmlDocument::GetHandle() const {
            return &m_impl;
        }

	private:
	    std::string m_content;
	    rapidxml::xml_document<> m_impl;
	};
}
