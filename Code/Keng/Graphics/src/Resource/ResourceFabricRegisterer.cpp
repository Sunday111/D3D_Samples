#include "ResourceFabricRegisterer.h"
#include "EverydayTools/Exception/CallAndRethrow.h"
#include "Keng/ResourceSystem/IResourceSystem.h"

#include "Resource/Shader/ShaderFabric.h"
#include "Resource/Effect/EffectFabric.h"
#include "Resource/Texture/TextureFabric.h"
#include "Resource/ShaderTemplate/ShaderTemplateFabric.h"
#include "Resource/Font/FontFabric.h"

namespace keng::graphics
{
    void ResourceFabricRegisterer::Register(const resource::IResourceSystemPtr& resourceSystem) {
        CallAndRethrowM + [&] {
            resourceSystem->RegisterResourceFabric(EffectFabricPtr::MakeInstance());
            resourceSystem->RegisterResourceFabric(ShaderFabricPtr::MakeInstance());
            resourceSystem->RegisterResourceFabric(ShaderTemplateFabricPtr::MakeInstance());
            resourceSystem->RegisterResourceFabric(TextureFabricPtr::MakeInstance());
            resourceSystem->RegisterResourceFabric(FontFabricPtr::MakeInstance());
        };
    }
}
