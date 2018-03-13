#pragma once

#include "ResourceSystem/ResourceSystem.h"
#include "D3D_Tools/Shader.h"

class Device;

using ShaderType = d3d_tools::ShaderType;
using ShaderVersion = d3d_tools::ShaderVersion;

class IShader :
    public IRefCountObject
{
public:
    virtual ~IShader() = default;
};

template<ShaderType st>
class Shader :
    public RefCountImpl<IShader>
{
public:
    d3d_tools::Shader<st> m_impl;
};

class ShaderTemplate :
    public RefCountImpl<IResource>
{
public:
    IntrusivePtr<Shader<ShaderType::Vertex>> vertexShader;
    IntrusivePtr<Shader<ShaderType::Pixel>> fragmentShader;
};

class ShaderTemplateResourceFabric :
    public RefCountImpl<IResourceFabric>
{
public:
    ShaderTemplateResourceFabric(IntrusivePtr<Device> device);

    virtual std::string_view GetResourceType() const override;
    virtual IntrusivePtr<IResource> LoadResource(IntrusivePtr<IXmlNode> document) const override;

private:
    IntrusivePtr<Device> m_device;
};