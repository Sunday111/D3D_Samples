#include "ShaderFabric.h"
#include "Resource/Shader/Shader.h"
#include "Resource/ShaderTemplate/ShaderTemplate.h"
#include "Keng/Graphics/GraphicsSystem.h"
#include "EverydayTools/Array/ArrayViewVector.h"
#include <vector>

namespace keng::graphics
{
    struct ShaderCompiler
    {
        core::Ptr<Device> device;
        core::Ptr<ShaderTemplate> shaderTemplate;
        std::string_view entryPoint;
        std::vector<d3d_tools::ShaderMacro> definitions;

        core::Ptr<resource::IResource> Compile() {
            switch (shaderTemplate->type) {
            case ShaderType::Vertex:
                return Compile<d3d_tools::ShaderType::Vertex>();

            case ShaderType::Fragment:
                return Compile<d3d_tools::ShaderType::Pixel>();

            default:
                throw std::runtime_error("Not implemented for this shader type here");
            }
        }

        template<d3d_tools::ShaderType st>
        core::Ptr<Shader<st>> Compile() {
            auto result = core::Ptr<Shader<st>>::MakeInstance();
            auto view =
                result->m_impl = device->CreateShader<st>(
                    shaderTemplate->code.c_str(),
                    entryPoint.data(),
                    d3d_tools::ShaderVersion::_5_0,
                    edt::MakeArrayView(definitions));
            return result;
        }
    };

    std::string_view ShaderFabric::GetNodeName() const {
        return "shader";
    }

    std::string_view ShaderFabric::GetResourceType() const {
        return "Shader";
    }

    core::Ptr<resource::IResource> ShaderFabric::LoadResource(resource::IResourceSystem* resourceSystem,
        const core::Ptr<IXmlNode>& node, const core::Ptr<resource::IDevice>& abstractDevice) const {
        return CallAndRethrowM + [&] {
            edt::ThrowIfFailed(abstractDevice != nullptr, "Can't create shader without device");
            auto device = std::dynamic_pointer_cast<Device>(abstractDevice);

            ShaderCompiler shaderCompiler;
            shaderCompiler.device = device;

            // Template
            auto templateNode = node->GetFirstNode("template");
            shaderCompiler.shaderTemplate = std::static_pointer_cast<ShaderTemplate>(resourceSystem->GetResource(templateNode->GetValue()));

            auto entryNode = node->GetFirstNode("entry_point");
            shaderCompiler.entryPoint = entryNode->GetValue();

            if (auto definitionsNode = node->FindFirstNode("definitions")) {
                for (
                    auto definitionNode = definitionsNode->FindFirstNode("definition");
                    definitionNode != nullptr;
                    definitionNode = definitionsNode->NextSibling()) {
                    d3d_tools::ShaderMacro macro;

                    auto definitionName = definitionNode->GetFirstNode("name");
                    macro.name = definitionName->GetValue();

                    if (auto definitionValue = definitionNode->FindFirstNode("value")) {
                        macro.value = definitionValue->GetValue();
                    }

                    shaderCompiler.definitions.push_back(macro);
                }
            }

            return shaderCompiler.Compile();
        };
    }
}
