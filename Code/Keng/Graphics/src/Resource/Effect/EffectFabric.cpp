#include "Device.h"
#include "Resource/Effect/EffectFabric.h"
#include "Resource/Effect/Effect.h"
#include "Keng/Base/Serialization/SerializeMandatory.h"
#include "yasli/STL.h"
#include "EverydayTools/Exception/ThrowIfFailed.h"

namespace keng::graphics
{
    const char* EffectFabric::GetNodeName() const {
        return "effect";
    }

    const char* EffectFabric::GetResourceType() const {
        return "Effect";
    }

    resource::IResourcePtr EffectFabric::LoadResource(resource::IResourceSystem& resourceSystem,
        Archive& ar, const resource::IDevicePtr& abstractDevice) const {
        return CallAndRethrowM + [&] {
            edt::ThrowIfFailed(abstractDevice != nullptr, "Can't create effect without device");
            auto device = std::dynamic_pointer_cast<Device>(abstractDevice);

            auto result = EffectPtr::MakeInstance();

            // TODO: check legal shader types combinations?

            struct EffectInfo {
                void serialize(Archive& ar) {
                    ar(vs, "vertex_shader");
                    ar(fs, "fragment_shader");
                }

                std::string vs;
                std::string fs;
            };

            EffectInfo effectInfo;
            SerializeMandatory(ar, effectInfo, GetNodeName());

            bool anyShader = false;

            {
                if (!effectInfo.vs.empty()) {
                    auto base = resourceSystem.GetResource(effectInfo.vs.data(), device);
                    result->vs = std::dynamic_pointer_cast<Shader<gpu::ShaderType::Vertex>>(base);
                    anyShader = true;
                }
            }

            {
                if (!effectInfo.fs.empty()) {
                    auto base = resourceSystem.GetResource(effectInfo.fs.data(), device);
                    result->fs = std::dynamic_pointer_cast<Shader<gpu::ShaderType::Fragment>>(base);
                    anyShader = true;
                }
            }

            edt::ThrowIfFailed(anyShader, "Invalid effect file (0 shaders)");
            return result;
        };
    }
}
