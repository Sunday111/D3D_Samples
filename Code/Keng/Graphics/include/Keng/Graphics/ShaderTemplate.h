#pragma once

#include "Keng/Graphics/Shader.h"

namespace keng
{
	class ShaderTemplate :
        public IResource
	{
	public:
		ShaderType type;
		std::string code;
	};

	class ShaderTemplateFabric :
		public IResourceFabric
	{
	public:
		virtual std::string_view GetNodeName() const override;
		virtual std::string_view GetResourceType() const override;
		virtual std::shared_ptr<IResource> LoadResource(IResourceSystem*, std::shared_ptr<IXmlNode> document) const override;
	};
}
