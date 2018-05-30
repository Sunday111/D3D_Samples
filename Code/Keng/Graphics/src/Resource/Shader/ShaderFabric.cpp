#include "ShaderFabric.h"
#include "Resource/ShaderTemplate/ShaderTemplate.h"
#include "EverydayTools/Array/ArrayViewVector.h"
#include <vector>
#include "yasli/STL.h"
#include "Keng/Base/Serialization/SerializeMandatory.h"
#include "EverydayTools/Exception/CallAndRethrow.h"
#include "EverydayTools/Exception/ThrowIfFailed.h"
#include "Device.h"
#include "Keng/GraphicsAPI/Shader/ShaderParameters.h"

namespace keng::graphics
{
    struct ShaderInfo {
        struct Definition {
            void serialize(Archive& ar) {
                SerializeMandatory(ar, name, "name");
                ar(value, "value");
            }
            std::string name;
            std::string value;
        };

        void serialize(Archive& ar) {
            SerializeMandatory(ar, shaderTemplate, "template");
            SerializeMandatory(ar, entryPoint, "entry_point");
            ar(definitions, "definitions");
        }

        std::string shaderTemplate;
        std::string entryPoint;
        std::vector<Definition> definitions;
    };

    struct ShaderCompiler
    {
        DevicePtr device;
        ShaderTemplatePtr shaderTemplate;
        std::string_view entryPoint;
        std::vector<graphics_api::ShaderDefinition> definitions;

        resource::IResourcePtr Compile() {
            switch (shaderTemplate->type) {
            case graphics_api::ShaderType::Vertex:
                return Compile<graphics_api::ShaderType::Vertex>();

            case graphics_api::ShaderType::Fragment:
                return Compile<graphics_api::ShaderType::Fragment>();

            default:
                throw std::runtime_error("Not implemented for this shader type here");
            }
        }

        template<graphics_api::ShaderType st>
        core::Ptr<Shader<st>> Compile() {
            auto result = core::Ptr<Shader<st>>::MakeInstance();
            graphics_api::ShaderParameters sp;
            sp.code = shaderTemplate->code.data();
            sp.definitions = edt::MakeArrayView(definitions);
            sp.entry_point = entryPoint.data();
            result->m_impl = std::dynamic_pointer_cast<graphics_api::IShaderT<st>>(device->GetApiDevice()->CreateShader(st, sp));
            return result;
        }
    };

    const char* ShaderFabric::GetNodeName() const {
        return "shader";
    }

    const char* ShaderFabric::GetResourceType() const {
        return "Shader";
    }

    resource::IResourcePtr ShaderFabric::LoadResource(resource::IResourceSystem& resourceSystem,
        Archive& ar, const resource::IDevicePtr& abstractDevice) const {
        return CallAndRethrowM + [&] {
            edt::ThrowIfFailed(abstractDevice != nullptr, "Can't create shader without device");
            auto device = std::dynamic_pointer_cast<Device>(abstractDevice);

            ShaderInfo info;
            SerializeMandatory(ar, info, GetNodeName());

            ShaderCompiler shaderCompiler;
            shaderCompiler.device = device;
            shaderCompiler.shaderTemplate = std::static_pointer_cast<ShaderTemplate>(resourceSystem.GetResource(info.shaderTemplate.data()));
            shaderCompiler.entryPoint = info.entryPoint;
            shaderCompiler.definitions.reserve(info.definitions.size());
            for (auto& def : info.definitions) {
                shaderCompiler.definitions.push_back(graphics_api::ShaderDefinition {
                    def.name.data(), def.value.data()
                });
            }

            return shaderCompiler.Compile();
        };
    }
}
