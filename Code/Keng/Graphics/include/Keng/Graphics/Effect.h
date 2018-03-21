#pragma once

#include "Keng/Graphics/Shader.h"

namespace keng::graphics
{
	class Effect :
        public resource::IResource
	{
	public:
		std::shared_ptr<Shader<ShaderType::Vertex>> vs;
		std::shared_ptr<Shader<ShaderType::Pixel>> fs;
	};

	class EffectFabric :
		public resource::IResourceFabric
	{
	public:
		virtual std::string_view GetNodeName() const override;
		virtual std::string_view GetResourceType() const override;
		virtual std::shared_ptr<resource::IResource> LoadResource(resource::IResourceSystem*, std::shared_ptr<IXmlNode> document) const override;
	};
}
