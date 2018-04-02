#include "ShaderTemplateFabric.h"
#include <fstream>
#include "EverydayTools/Exception/CallAndRethrow.h"
#include "Resource/ShaderTemplate/ShaderTemplate.h"
#include "yasli/STL.h"
#include "EverydayTools/Exception/ThrowIfFailed.h"
#include "keng/ResourceSystem/OpenArchiveJSON.h"

namespace keng::graphics
{
    std::string_view ShaderTemplateFabric::GetNodeName() const {
        return "shader_template";
    }

    std::string_view ShaderTemplateFabric::GetResourceType() const {
        return "ShaderTemplate";
    }

    core::Ptr<resource::IResource> ShaderTemplateFabric::LoadResource(resource::IResourceSystem*,
        yasli::Archive& ar, const core::Ptr<resource::IDevice>&) const {
        return CallAndRethrowM + [&] {
            struct ShaderTemplateInfo {
                void serialize(yasli::Archive& ar) {
                    resource::SerializeMandatory(ar, type, "type");
                    resource::SerializeMandatory(ar, filename, "file");
                }
                ShaderType type;
                std::string filename;
            };

            ShaderTemplateInfo info;
            resource::SerializeMandatory(ar, info, GetNodeName().data());

            std::ifstream shaderFile;
            shaderFile.open(info.filename.data());
            edt::ThrowIfFailed(shaderFile.is_open(), "Could not open file \"", info.filename, "\"");

            auto content = std::string(
                (std::istreambuf_iterator<char>(shaderFile)),
                (std::istreambuf_iterator<char>()));

            auto shaderTemplate = core::Ptr<ShaderTemplate>::MakeInstance();
            shaderTemplate->type = info.type;
            shaderTemplate->code = std::move(content);

            return shaderTemplate;
        };
    }
}
