#pragma once

#include "Shader.h"

class ShaderTemplate :
	public SimpleResourceBase
{
public:
	ShaderType type;
	std::string code;
};

class ShaderTemplateFabric :
    public RefCountImpl<IResourceFabric>
{
public:
	virtual std::string_view GetNodeName() const override;
    virtual std::string_view GetResourceType() const override;
    virtual IntrusivePtr<IResource> LoadResource(IResourceSystem*, IntrusivePtr<IXmlNode> document) const override;
};