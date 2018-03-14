#include "Shader.h"
#include "GraphicsSystem.h"
#include <fstream>

template<typename Parent>
IntrusivePtr<IXmlNode> GetMandatoryNode(Parent& parent, std::string_view name) {
	return CallAndRethrowM + [&] {
		auto node = parent->FindFirstNode(name);
		if (node == nullptr) {
			throw std::runtime_error("Could not find node");
		}
		return node;
	};
}

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
		auto typeNode = GetMandatoryNode(node, "type");
		auto fileNode = GetMandatoryNode(node, "file");

		auto shaderType = ParseShaderType(typeNode->GetValue());

		std::ifstream shaderFile;
		shaderFile.open(fileNode->GetValue().data());
		if (!shaderFile.is_open()) {
			throw std::runtime_error("File not found");
		}

		auto content = std::string(
			(std::istreambuf_iterator<char>(shaderFile)),
			(std::istreambuf_iterator<char>()));

		auto shaderTemplate = IntrusivePtr<ShaderTemplate>::MakeInstance();
		shaderTemplate->type = shaderType;
		shaderTemplate->code = std::move(content);

		return shaderTemplate;
    };
}

ShaderFabric::ShaderFabric(IntrusivePtr<Device> device) :
	m_device(device)
{}

std::string_view ShaderFabric::GetNodeName() const {
	return "shader";
}

std::string_view ShaderFabric::GetResourceType() const {
	return "Shader";
}

template<ShaderType st>
IntrusivePtr<Shader<st>> LoadShader(IntrusivePtr<ShaderTemplate> shaderTemplate, std::string_view entryPoint, IntrusivePtr<Device> device) {
	auto result = IntrusivePtr<Shader<st>>::MakeInstance();
	result->m_impl = device->CreateShader<st>(
		shaderTemplate->code.c_str(),
		entryPoint.data(),
		d3d_tools::ShaderVersion::_5_0);
	return result;
}

IntrusivePtr<IResource> ShaderFabric::LoadResource(IResourceSystem* resourceSystem, IntrusivePtr<IXmlNode> node) const {
	return CallAndRethrowM + [&] () -> IntrusivePtr<IResource> {
		auto templateNode = GetMandatoryNode(node, "template");
		auto entryNode = GetMandatoryNode(node, "entry_point");
		auto shaderTemplate = std::dynamic_pointer_cast<ShaderTemplate>(resourceSystem->GetResource(templateNode->GetValue()));
		auto entryPoint = entryNode->GetValue();

		switch (shaderTemplate->type)
		{
		case ShaderType::Vertex:
			return LoadShader<ShaderType::Vertex>(shaderTemplate, entryPoint, m_device);

		case ShaderType::Pixel:
			return LoadShader<ShaderType::Pixel>(shaderTemplate, entryPoint, m_device);

		default:
			throw std::runtime_error("Not implemented for this shader type here");
		}
	};
}

std::string_view EffectFabric::GetNodeName() const {
	return "effect";
}

std::string_view EffectFabric::GetResourceType() const {
	return "Effect";
}

IntrusivePtr<IResource> EffectFabric::LoadResource(IResourceSystem* resourceSystem, IntrusivePtr<IXmlNode> node) const {
	return CallAndRethrowM + [&] {
		bool anyShader = false;
		auto result = IntrusivePtr<Effect>::MakeInstance();

		// TODO: check legal shader types combinations?

		{
			auto vsNode = GetMandatoryNode(node, "vertex_shader");
			if (vsNode != nullptr) {
				result->vs = std::dynamic_pointer_cast<Shader<ShaderType::Vertex>>(resourceSystem->GetResource(vsNode->GetValue()));
				anyShader = true;
			}
		}

		{
			auto vsNode = GetMandatoryNode(node, "fragment_shader");
			if (vsNode != nullptr) {
				result->fs = std::dynamic_pointer_cast<Shader<ShaderType::Pixel>>(resourceSystem->GetResource(vsNode->GetValue()));
				anyShader = true;
			}
		}

		if (!anyShader) {
			throw std::runtime_error("Invalid effect file (0 shaders)");
		}

		return result;
	};
}

