#pragma once

#include "Keng/Graphics/Shader.h"

namespace keng
{
	class ShaderTemplate :
        public RefCountImpl<IResource>
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
}
