#pragma once

#include "Keng/Resource/ResourceSystem.h"
#include "D3D_Tools/Shader.h"

namespace keng
{
	class Device;
	using ShaderType = d3d_tools::ShaderType;
	using ShaderVersion = d3d_tools::ShaderVersion;

	template<ShaderType shaderType>
	class Shader :
        public RefCountImpl<IResource>
	{
	public:
		d3d_tools::Shader<shaderType> m_impl;
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
}
