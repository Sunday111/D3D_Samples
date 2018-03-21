#pragma once

#include "Keng/ResourceSystem/IResourceFabric.h"
#include "Keng/ResourceSystem/IResourceSystem.h"
#include "Keng/ResourceSystem/IResource.h"
#include "D3D_Tools/Shader.h"

namespace keng
{
	class Device;
	using ShaderType = d3d_tools::ShaderType;
	using ShaderVersion = d3d_tools::ShaderVersion;

	template<ShaderType shaderType>
	class Shader :
        public IResource
	{
	public:
		d3d_tools::Shader<shaderType> m_impl;
	};

	class ShaderFabric :
		public IResourceFabric
	{
	public:
		ShaderFabric(std::shared_ptr<Device>);

		virtual std::string_view GetNodeName() const override;
		virtual std::string_view GetResourceType() const override;
		virtual std::shared_ptr<IResource> LoadResource(IResourceSystem*, std::shared_ptr<IXmlNode> document) const override;

	private:
        std::shared_ptr<Device> m_device;
	};
}
