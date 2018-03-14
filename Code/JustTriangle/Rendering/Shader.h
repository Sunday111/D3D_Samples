#pragma once

#include "ResourceSystem/ResourceSystem.h"
#include "D3D_Tools/Shader.h"

class Device;

using ShaderType = d3d_tools::ShaderType;
using ShaderVersion = d3d_tools::ShaderVersion;

class ShaderTemplate :
	public RefCountImpl<IResource>
{
public:
	ShaderType type;
	std::string code;
};

template<ShaderType shaderType>
class Shader :
	public RefCountImpl<IResource>
{
public:
	d3d_tools::Shader<shaderType> m_impl;
};

class Effect :
	public RefCountImpl<IResource>
{
public:
	IntrusivePtr<Shader<ShaderType::Vertex>> vs;
	IntrusivePtr<Shader<ShaderType::Pixel>> fs;
};

class ShaderTemplateFabric :
    public RefCountImpl<IResourceFabric>
{
public:
	virtual std::string_view GetNodeName() const override;
    virtual std::string_view GetResourceType() const override;
    virtual IntrusivePtr<IResource> LoadResource(IResourceSystem*, IntrusivePtr<IXmlNode> document) const override;
};

class ShaderFabric :
	public RefCountImpl<IResourceFabric>
{
public:
	ShaderFabric(IntrusivePtr<Device>);

	virtual std::string_view GetNodeName() const override;
	virtual std::string_view GetResourceType() const override;
	virtual IntrusivePtr<IResource> LoadResource(IResourceSystem*, IntrusivePtr<IXmlNode> document) const override;

private:
	IntrusivePtr<Device> m_device;
};

class EffectFabric :
	public RefCountImpl<IResourceFabric>
{
public:
	virtual std::string_view GetNodeName() const override;
	virtual std::string_view GetResourceType() const override;
	virtual IntrusivePtr<IResource> LoadResource(IResourceSystem*, IntrusivePtr<IXmlNode> document) const override;
};

