#include "Keng/Graphics/Device.h"
#include "Resource/Effect/EffectFabric.h"
#include "Resource/Effect/Effect.h"
#include "Xml.h"

namespace keng::graphics
{
    std::string_view EffectFabric::GetNodeName() const {
        return "effect";
    }

    std::string_view EffectFabric::GetResourceType() const {
        return "Effect";
    }

    core::Ptr<resource::IResource> EffectFabric::LoadResource(resource::IResourceSystem* resourceSystem,
        const core::Ptr<IXmlNode>& node, const core::Ptr<resource::IDevice>& abstractDevice) const {
        return CallAndRethrowM + [&] {
            edt::ThrowIfFailed(abstractDevice != nullptr, "Can't create effect without device");
            auto device = std::dynamic_pointer_cast<Device>(abstractDevice);

            bool anyShader = false;
            auto result = core::Ptr<Effect>::MakeInstance();

            // TODO: check legal shader types combinations?

            {
                auto vsNode = node->FindFirstNode("vertex_shader");
                if (vsNode != nullptr) {
                    result->vs = std::dynamic_pointer_cast<Shader<d3d_tools::ShaderType::Vertex>>(resourceSystem->GetResource(vsNode->GetValue(), device));
                    anyShader = true;
                }
            }

            {
                auto vsNode = node->FindFirstNode("fragment_shader");
                if (vsNode != nullptr) {
                    result->fs = std::dynamic_pointer_cast<Shader<d3d_tools::ShaderType::Pixel>>(resourceSystem->GetResource(vsNode->GetValue(), device));
                    anyShader = true;
                }
            }

            edt::ThrowIfFailed(anyShader, "Invalid effect file (0 shaders)");
            result->device = device;
            return result;
        };
    }
}
