#include "Keng/Graphics/Shader.h"
#include "Keng/Graphics/ShaderTemplate.h"
#include "Keng/Graphics/Effect.h"
#include "Keng/Graphics/GraphicsSystem.h"
#include "EverydayTools/Array/ArrayViewVector.h"
#include <fstream>

namespace keng::graphics
{
	struct ShaderCompiler
	{
		std::shared_ptr<Device> device;
		std::shared_ptr<ShaderTemplate> shaderTemplate;
		std::string_view entryPoint;
		std::vector<d3d_tools::ShaderMacro> definitions;

		std::shared_ptr<resource::IResource> Compile() {
			switch (shaderTemplate->type)
			{
			case ShaderType::Vertex:
				return Compile<ShaderType::Vertex>();

			case ShaderType::Pixel:
				return Compile<ShaderType::Pixel>();

			default:
				throw std::runtime_error("Not implemented for this shader type here");
			}
		}

		template<ShaderType st>
		std::shared_ptr<Shader<st>> Compile() {
			auto result = std::make_shared<Shader<st>>();
			auto view = 
			result->m_impl = device->CreateShader<st>(
				shaderTemplate->code.c_str(),
				entryPoint.data(),
				d3d_tools::ShaderVersion::_5_0,
				edt::MakeArrayView(definitions));
			return result;
		}
	};

	ShaderFabric::ShaderFabric(std::shared_ptr<Device> device) :
		m_device(device)
	{}

	std::string_view ShaderFabric::GetNodeName() const {
		return "shader";
	}

	std::string_view ShaderFabric::GetResourceType() const {
		return "Shader";
	}

	std::shared_ptr<resource::IResource> ShaderFabric::LoadResource(resource::IResourceSystem* resourceSystem, std::shared_ptr<IXmlNode> node) const {
		return CallAndRethrowM + [&]() -> std::shared_ptr<resource::IResource> {
			ShaderCompiler shaderCompiler;
			shaderCompiler.device = m_device;

			// Template
			auto templateNode = node->GetFirstNode("template");
			shaderCompiler.shaderTemplate = std::dynamic_pointer_cast<ShaderTemplate>(resourceSystem->GetResource(templateNode->GetValue()));

			auto entryNode = node->GetFirstNode("entry_point");
			shaderCompiler.entryPoint = entryNode->GetValue();

			if (auto definitionsNode = node->FindFirstNode("definitions")) {
                for (
                    auto definitionNode = definitionsNode->FindFirstNode("definition");
                    definitionNode != nullptr;
                    definitionNode = definitionsNode->NextSibling())
                {
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
