#include "ShaderTemplateFabric.h"
#include <fstream>
#include "EverydayTools/Exception/CallAndRethrow.h"
#include "Xml.h"
#include "Resource/ShaderTemplate/ShaderTemplate.h"

namespace keng::graphics
{
    namespace
    {
        struct Test
        {
            void serialize(yasli::Archive& ar) {
                ar(a, "a", "a");
            }

            int a = 10;
        };
    }

    static ShaderType ParseShaderType(std::string_view str) {
        return CallAndRethrowM + [&] {
            if (str == "Vertex") return ShaderType::Vertex;
            if (str == "Pixel") return ShaderType::Fragment;
            if (str == "Fragment") return ShaderType::Fragment;
            throw std::runtime_error("Failed to parse shader type");
        };
    }

    std::string_view ShaderTemplateFabric::GetNodeName() const {
        return "shader_template";
    }

    std::string_view ShaderTemplateFabric::GetResourceType() const {
        return "ShaderTemplate";
    }

    core::Ptr<resource::IResource> ShaderTemplateFabric::LoadResource(resource::IResourceSystem*,
        const core::Ptr<IXmlNode>& node, const core::Ptr<resource::IDevice>&) const {
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

            auto shaderTemplate = core::Ptr<ShaderTemplate>::MakeInstance();
            shaderTemplate->type = shaderType;
            shaderTemplate->code = std::move(content);

            return shaderTemplate;
        };
    }
}
