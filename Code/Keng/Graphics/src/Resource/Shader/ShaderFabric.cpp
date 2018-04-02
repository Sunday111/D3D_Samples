#include "ShaderFabric.h"
#include "Resource/Shader/Shader.h"
#include "Resource/ShaderTemplate/ShaderTemplate.h"
#include "Keng/Graphics/GraphicsSystem.h"
#include "EverydayTools/Array/ArrayViewVector.h"
#include <vector>
#include "yasli/STL.h"
#include "keng/ResourceSystem/OpenArchiveJSON.h"

namespace keng::graphics
{
    struct ShaderInfo {
        struct Definition {
            void serialize(yasli::Archive& ar) {
                resource::SerializeMandatory(ar, name, "name");
                ar(value, "value");
            }
            std::string name;
            std::string value;
        };

        void serialize(yasli::Archive& ar) {
            resource::SerializeMandatory(ar, shaderTemplate, "template");
            resource::SerializeMandatory(ar, entryPoint, "entry_point");
            ar(definitions, "definitions");
        }

        std::string shaderTemplate;
        std::string entryPoint;
        std::vector<Definition> definitions;
    };

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
        yasli::Archive& ar, const core::Ptr<resource::IDevice>& abstractDevice) const {
        return CallAndRethrowM + [&] {
            edt::ThrowIfFailed(abstractDevice != nullptr, "Can't create shader without device");
            auto device = std::dynamic_pointer_cast<Device>(abstractDevice);

            ShaderInfo info;
            resource::SerializeMandatory(ar, info, GetNodeName().data());

            ShaderCompiler shaderCompiler;
            shaderCompiler.device = device;
            shaderCompiler.shaderTemplate = std::static_pointer_cast<ShaderTemplate>(resourceSystem->GetResource(info.shaderTemplate));
            shaderCompiler.entryPoint = info.entryPoint;
            shaderCompiler.definitions.reserve(info.definitions.size());
            for (auto& def : info.definitions) {
                shaderCompiler.definitions.push_back(d3d_tools::ShaderMacro {
                    def.name, def.value
                });
            }

            return shaderCompiler.Compile();
        };
    }
}
