#include "Keng/Graphics/Shader.h"
#include "Keng/Graphics/ShaderTemplate.h"
#include "Keng/Graphics/Effect.h"
#include "Keng/Graphics/GraphicsSystem.h"
#include "EverydayTools/Array/ArrayViewVector.h"
#include <fstream>

namespace keng
{
	struct ShaderCompiler
	{
		IntrusivePtr<Device> device;
		IntrusivePtr<ShaderTemplate> shaderTemplate;
		std::string_view entryPoint;
		std::vector<d3d_tools::ShaderMacro> definitions;

		IntrusivePtr<IResource> Compile() {
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
		IntrusivePtr<Shader<st>> Compile() {
			auto result = IntrusivePtr<Shader<st>>::MakeInstance();
			auto view = 
			result->m_impl = device->CreateShader<st>(
				shaderTemplate->code.c_str(),
				entryPoint.data(),
				d3d_tools::ShaderVersion::_5_0,
				edt::MakeArrayView(definitions));
			return result;
		}
	};

	ShaderFabric::ShaderFabric(IntrusivePtr<Device> device) :
		m_device(device)
	{}

	std::string_view ShaderFabric::GetNodeName() const {
		return "shader";
	}

	std::string_view ShaderFabric::GetResourceType() const {
		return "Shader";
	}

	IntrusivePtr<IResource> ShaderFabric::LoadResource(IResourceSystem* resourceSystem, IntrusivePtr<IXmlNode> node) const {
		return CallAndRethrowM + [&]() -> IntrusivePtr<IResource> {
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
