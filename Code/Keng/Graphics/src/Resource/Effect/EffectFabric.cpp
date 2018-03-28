#include "Keng/Graphics/Device.h"
#include "Resource/Effect/EffectFabric.h"
#include "Resource/Effect/Effect.h"
#include "Xml.h"

namespace keng::graphics
{
    EffectFabric::EffectFabric(core::Ptr<Device> device) :
        m_device(device) {
    }

    EffectFabric::~EffectFabric() = default;

    std::string_view EffectFabric::GetNodeName() const {
        return "effect";
    }

    std::string_view EffectFabric::GetResourceType() const {
        return "Effect";
    }

    core::Ptr<resource::IResource> EffectFabric::LoadResource(resource::IResourceSystem* resourceSystem, core::Ptr<IXmlNode> node) const {
        return CallAndRethrowM + [&] {
            bool anyShader = false;
            auto result = core::Ptr<Effect>::MakeInstance();

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
            result->device = m_device;
            return result;
        };
    }
}
