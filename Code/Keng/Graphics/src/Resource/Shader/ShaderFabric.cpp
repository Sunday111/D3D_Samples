#include "ShaderFabric.h"
#include "Resource/ShaderTemplate/ShaderTemplate.h"
#include "EverydayTools/Array/ArrayViewVector.h"
#include <vector>
#include "yasli/STL.h"
#include "Keng/Base/Serialization/SerializeMandatory.h"
#include "EverydayTools/Exception/CallAndRethrow.h"
#include "EverydayTools/Exception/ThrowIfFailed.h"
#include "Device.h"
#include "Keng/GPU/Shader/ShaderParameters.h"

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
        std::vector<gpu::ShaderDefinition> definitions;

        resource::IResourcePtr Compile() {
            switch (shaderTemplate->type) {
            case ShaderType::Vertex:
                return Compile<ShaderType::Vertex>();

            case ShaderType::Fragment:
                return Compile<ShaderType::Fragment>();

            default:
                throw std::runtime_error("Not implemented for this shader type here");
            }
        }

        template<ShaderType st>
        core::Ptr<Shader<st>> Compile() {
            auto result = core::Ptr<Shader<st>>::MakeInstance();
            gpu::ShaderParameters sp;
            sp.code = shaderTemplate->code.data();
            sp.definitions = edt::MakeArrayView(definitions);
            sp.entry_point = entryPoint.data();
            result->m_impl = std::dynamic_pointer_cast<gpu::IShaderT<st>>(device->GetApiDevice()->CreateShader(st, sp));
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
                shaderCompiler.definitions.push_back(gpu::ShaderDefinition {
                    def.name.data(), def.value.data()
                });
            }

            return shaderCompiler.Compile();
        };
    }
}
