#include "Resource/Effect/EffectFabric.h"
#include "Resource/Effect/Effect.h"

namespace keng::graphics
{
    std::string_view EffectFabric::GetNodeName() const {
        return "effect";
    }

    std::string_view EffectFabric::GetResourceType() const {
        return "Effect";
    }

    std::shared_ptr<resource::IResource> EffectFabric::LoadResource(resource::IResourceSystem* resourceSystem, std::shared_ptr<IXmlNode> node) const {
        return CallAndRethrowM + [&] {
            bool anyShader = false;
            auto result = std::make_shared<Effect>();

            // TODO: check legal shader types combinations?

            {
                auto vsNode = node->FindFirstNode("vertex_shader");
                if (vsNode != nullptr) {
                    result->vs = std::dynamic_pointer_cast<Shader<ShaderType::Vertex>>(resourceSystem->GetResource(vsNode->GetValue()));
                    anyShader = true;
                }
            }

            {
                auto vsNode = node->FindFirstNode("fragment_shader");
                if (vsNode != nullptr) {
                    result->fs = std::dynamic_pointer_cast<Shader<ShaderType::Pixel>>(resourceSystem->GetResource(vsNode->GetValue()));
                    anyShader = true;
                }
            }

            edt::ThrowIfFailed(anyShader, "Invalid effect file (0 shaders)");

            return result;
        };
    }
}
