#include "ResourceFabricRegisterer.h"
#include "EverydayTools/Exception/CallAndRethrow.h"
#include "Keng/ResourceSystem/IResourceSystem.h"
#include "Keng/Graphics/GraphicsSystem.h"

#include "Resource/Shader/ShaderFabric.h"
#include "Resource/Effect/EffectFabric.h"
#include "Resource/Texture/TextureFabric.h"
#include "Resource/ShaderTemplate/ShaderTemplateFabric.h"

namespace keng::graphics
{
    ResourceFabricRegisterer::ResourceFabricRegisterer(GraphicsSystem* graphicsSystem) :
        m_graphicsSystem(graphicsSystem)
    {
        assert(m_graphicsSystem);
    }

    void ResourceFabricRegisterer::Register(resource::IResourceSystem* resourceSystem) {
        CallAndRethrowM + [&] {
            auto device = m_graphicsSystem->GetDevice();
            resourceSystem->RegisterResourceFabric(std::make_shared<EffectFabric>());
            resourceSystem->RegisterResourceFabric(std::make_shared<ShaderFabric>(device));
            resourceSystem->RegisterResourceFabric(std::make_shared<ShaderTemplateFabric>());
            resourceSystem->RegisterResourceFabric(std::make_shared<TextureFabric>(device));
        };
    }
}
