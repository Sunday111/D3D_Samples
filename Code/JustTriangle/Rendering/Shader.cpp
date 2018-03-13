#include "Shader.h"
#include "GraphicsSystem.h"
#include <fstream>

ShaderTemplateResourceFabric::ShaderTemplateResourceFabric(IntrusivePtr<Device> device) :
    m_device(device)
{}

std::string_view ShaderTemplateResourceFabric::GetResourceType() const {
    return "ShaderTemplate";
}

template<ShaderType st>
IntrusivePtr<Shader<st>> LoadShader(IntrusivePtr<Device> device, IntrusivePtr<IXmlNode> node) {
    if (node == nullptr) {
        return nullptr;
    }

    auto file_node = node->FindFirstNode("file");
    if (file_node == nullptr) {
        throw std::runtime_error("Could not find file node");
    }

    auto entry_node = node->FindFirstNode("entry_point");
    if (entry_node == nullptr) {
        throw std::runtime_error("Could not find file entry_point node");
    }

    std::ifstream file;
    file.open(file_node->GetValue().data());
    if (!file.is_open()) {
        throw std::runtime_error("Could not find file for vertex shader!");
    }

    auto result = IntrusivePtr<Shader<st>>::MakeInstance();
    result->m_impl = device->CreateShader<st>(file, entry_node->GetValue().data(), d3d_tools::ShaderVersion::_5_0);
    return result;
}


IntrusivePtr<IResource> ShaderTemplateResourceFabric::LoadResource(IntrusivePtr<IXmlNode> document) const {
    return CallAndRethrowM + [&] {
        auto shaderTemplate = IntrusivePtr<ShaderTemplate>::MakeInstance();
        shaderTemplate->vertexShader = LoadShader<ShaderType::Vertex>(m_device, document->FindFirstNode("vertex_shader"));
        shaderTemplate->fragmentShader = LoadShader<ShaderType::Pixel>(m_device, document->FindFirstNode("fragment_shader"));
        return shaderTemplate;
    };
}

