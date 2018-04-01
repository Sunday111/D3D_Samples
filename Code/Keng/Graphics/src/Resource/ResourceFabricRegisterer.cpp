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
    void ResourceFabricRegisterer::Register(resource::IResourceSystem* resourceSystem) {
        CallAndRethrowM + [&] {
            resourceSystem->RegisterResourceFabric(core::Ptr<EffectFabric>::MakeInstance());
            resourceSystem->RegisterResourceFabric(core::Ptr<ShaderFabric>::MakeInstance());
            resourceSystem->RegisterResourceFabric(core::Ptr<ShaderTemplateFabric>::MakeInstance());
            resourceSystem->RegisterResourceFabric(core::Ptr<TextureFabric>::MakeInstance());
        };
    }
}
