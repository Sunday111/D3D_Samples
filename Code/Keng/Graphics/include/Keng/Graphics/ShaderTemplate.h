#pragma once

#include "Keng/Graphics/Shader.h"

namespace keng::graphics
{
	class ShaderTemplate :
        public resource::IResource
	{
	public:
		ShaderType type;
		std::string code;
	};

	class ShaderTemplateFabric :
		public resource::IResourceFabric
	{
	public:
		virtual std::string_view GetNodeName() const override;
		virtual std::string_view GetResourceType() const override;
		virtual std::shared_ptr<resource::IResource> LoadResource(resource::IResourceSystem*, std::shared_ptr<IXmlNode> document) const override;
	};
}
