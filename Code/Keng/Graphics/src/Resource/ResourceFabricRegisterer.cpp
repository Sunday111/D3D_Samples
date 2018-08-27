#include "ResourceFabricRegisterer.h"
#include "EverydayTools/Exception/CallAndRethrow.h"
#include "EverydayTools/Exception/ThrowIfFailed.h"
#include "Keng/Core/IApplication.h"
#include "Keng/FileSystem/IFileSystem.h"
#include "Keng/ResourceSystem/IResourceSystem.h"

#include "Resource/Shader/ShaderFabric.h"
#include "Resource/Effect/EffectFabric.h"
#include "Resource/Texture/TextureFabric.h"
#include "Resource/ShaderTemplate/ShaderTemplateFabric.h"
#include "Resource/Font/FontFabric.h"

namespace keng::graphics
{
    void ResourceFabricRegisterer::Register(core::IApplication& app) {
        CallAndRethrowM + [&] {
            auto resourceSystem = app.FindSystem<resource::IResourceSystem>();
            edt::ThrowIfFailed(resourceSystem != nullptr, "IResourceSystem is not found");

            auto fileSystem = app.FindSystem<filesystem::IFileSystem>();
            edt::ThrowIfFailed(resourceSystem != nullptr, "IFileSystem is not found");

            resourceSystem->RegisterResourceFabric(EffectFabricPtr::MakeInstance());
            resourceSystem->RegisterResourceFabric(ShaderFabricPtr::MakeInstance());
            resourceSystem->RegisterResourceFabric(ShaderTemplateFabricPtr::MakeInstance());
            resourceSystem->RegisterResourceFabric(TextureFabricPtr::MakeInstance());
            resourceSystem->RegisterResourceFabric(FontFabricPtr::MakeInstance(fileSystem));
        };
    }
}
