#include "Keng/Core/Rendering/ShaderTemplate.h"
#include "EverydayTools/Exception/ThrowIfFailed.h"

namespace keng
{
	static ShaderType ParseShaderType(std::string_view str) {
		return CallAndRethrowM + [&] {
			if (str == "Vertex") return ShaderType::Vertex;
			if (str == "Pixel") return ShaderType::Pixel;
			if (str == "Fragment") return ShaderType::Pixel;
			throw std::runtime_error("Failed to parse shader type");
		};
	}

	std::string_view ShaderTemplateFabric::GetNodeName() const {
		return "shader_template";
	}

	std::string_view ShaderTemplateFabric::GetResourceType() const {
		return "ShaderTemplate";
	}

	IntrusivePtr<IResource> ShaderTemplateFabric::LoadResource(IResourceSystem*, IntrusivePtr<IXmlNode> node) const {
		return CallAndRethrowM + [&] {
			auto typeNode = node->GetFirstNode("type");
			auto fileNode = node->GetFirstNode("file");

			auto shaderType = ParseShaderType(typeNode->GetValue());

			auto shaderFilename = fileNode->GetValue();
			std::ifstream shaderFile;
			shaderFile.open(shaderFilename.data());
			edt::ThrowIfFailed(shaderFile.is_open(), "Could not open file \"", shaderFilename, "\"");

			auto content = std::string(
				(std::istreambuf_iterator<char>(shaderFile)),
				(std::istreambuf_iterator<char>()));

			auto shaderTemplate = IntrusivePtr<ShaderTemplate>::MakeInstance();
			shaderTemplate->type = shaderType;
			shaderTemplate->code = std::move(content);

			return shaderTemplate;
		};
	}
}
