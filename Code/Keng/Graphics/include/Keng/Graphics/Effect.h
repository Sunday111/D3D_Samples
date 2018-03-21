#pragma once

#include "Keng/Graphics/Shader.h"

namespace keng
{
	class Effect :
        public IResource
	{
	public:
		std::shared_ptr<Shader<ShaderType::Vertex>> vs;
		std::shared_ptr<Shader<ShaderType::Pixel>> fs;
	};

	class EffectFabric :
		public IResourceFabric
	{
	public:
		virtual std::string_view GetNodeName() const override;
		virtual std::string_view GetResourceType() const override;
		virtual std::shared_ptr<IResource> LoadResource(IResourceSystem*, std::shared_ptr<IXmlNode> document) const override;
	};
}
