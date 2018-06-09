#include "ShaderTemplateFabric.h"
#include <fstream>
#include "EverydayTools/Exception/CallAndRethrow.h"
#include "Resource/ShaderTemplate/ShaderTemplate.h"
#include "yasli/STL.h"
#include "EverydayTools/Exception/ThrowIfFailed.h"
#include "Keng/Base/Serialization/SerializeMandatory.h"

namespace keng::graphics
{
    const char* ShaderTemplateFabric::GetNodeName() const {
        return "shader_template";
    }

    const char* ShaderTemplateFabric::GetResourceType() const {
        return "ShaderTemplate";
    }

    resource::IResourcePtr ShaderTemplateFabric::LoadResource(resource::IResourceSystem&,
        Archive& ar, const resource::IDevicePtr&) const {
        return CallAndRethrowM + [&] {
            struct ShaderTemplateInfo {
                void serialize(Archive& ar) {
                    SerializeMandatory(ar, type, "type");
                    SerializeMandatory(ar, filename, "file");
                }
                ShaderType type;
                std::string filename;
            };

            ShaderTemplateInfo info;
            SerializeMandatory(ar, info, GetNodeName());

            std::ifstream shaderFile;
            shaderFile.open(info.filename.data());
            edt::ThrowIfFailed(shaderFile.is_open(), "Could not open file \"", info.filename, "\"");

            auto content = std::string(
                (std::istreambuf_iterator<char>(shaderFile)),
                (std::istreambuf_iterator<char>()));

            auto shaderTemplate = ShaderTemplatePtr::MakeInstance();
            shaderTemplate->type = info.type;
            shaderTemplate->code = std::move(content);

            return shaderTemplate;
        };
    }
}
