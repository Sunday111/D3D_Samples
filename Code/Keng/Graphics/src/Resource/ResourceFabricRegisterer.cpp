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
            resourceSystem->RegisterResourceFabric(core::Ptr<EffectFabric>::MakeInstance(device));
            resourceSystem->RegisterResourceFabric(core::Ptr<ShaderFabric>::MakeInstance(device));
            resourceSystem->RegisterResourceFabric(core::Ptr<ShaderTemplateFabric>::MakeInstance());
            resourceSystem->RegisterResourceFabric(core::Ptr<TextureFabric>::MakeInstance(device));
        };
    }
}
