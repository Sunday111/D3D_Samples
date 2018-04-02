#include "Keng/Graphics/Device.h"
#include "Resource/Effect/EffectFabric.h"
#include "Resource/Effect/Effect.h"
#include "keng/ResourceSystem/OpenArchiveJSON.h"
#include "yasli/STL.h"

namespace keng::graphics
{
    std::string_view EffectFabric::GetNodeName() const {
        return "effect";
    }

    std::string_view EffectFabric::GetResourceType() const {
        return "Effect";
    }

    core::Ptr<resource::IResource> EffectFabric::LoadResource(resource::IResourceSystem* resourceSystem,
        yasli::Archive& ar, const core::Ptr<resource::IDevice>& abstractDevice) const {
        return CallAndRethrowM + [&] {
            edt::ThrowIfFailed(abstractDevice != nullptr, "Can't create effect without device");
            auto device = std::dynamic_pointer_cast<Device>(abstractDevice);

            auto result = core::Ptr<Effect>::MakeInstance();

            // TODO: check legal shader types combinations?

            struct EffectInfo {
                void serialize(yasli::Archive& ar) {
                    ar(vs, "vertex_shader");
                    ar(fs, "fragment_shader");
                }

                std::string vs;
                std::string fs;
            };

            EffectInfo effectInfo;
            resource::SerializeMandatory(ar, effectInfo, GetNodeName().data());

            bool anyShader = false;

            {
                if (!effectInfo.vs.empty()) {
                    result->vs = std::dynamic_pointer_cast<Shader<d3d_tools::ShaderType::Vertex>>(resourceSystem->GetResource(effectInfo.vs, device));
                    anyShader = true;
                }
            }

            {
                if (!effectInfo.fs.empty()) {
                    result->fs = std::dynamic_pointer_cast<Shader<d3d_tools::ShaderType::Pixel>>(resourceSystem->GetResource(effectInfo.fs, device));
                    anyShader = true;
                }
            }

            edt::ThrowIfFailed(anyShader, "Invalid effect file (0 shaders)");
            result->device = device;
            return result;
        };
    }
}
