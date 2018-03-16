#pragma once

#include "Shader.h"

namespace keng
{
	class Effect :
		public SimpleResourceBase
	{
	public:
		IntrusivePtr<Shader<ShaderType::Vertex>> vs;
		IntrusivePtr<Shader<ShaderType::Pixel>> fs;
	};

	class EffectFabric :
		public RefCountImpl<IResourceFabric>
	{
	public:
		virtual std::string_view GetNodeName() const override;
		virtual std::string_view GetResourceType() const override;
		virtual IntrusivePtr<IResource> LoadResource(IResourceSystem*, IntrusivePtr<IXmlNode> document) const override;
	};
}
