#include "Shader.h"
#include "GraphicsSystem.h"
#include <fstream>

#include "ShaderTemplate.h"
#include "Effect.h"

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
        auto templateNode = node->GetFirstNode("template");
        auto entryNode = node->GetFirstNode("entry_point");
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

