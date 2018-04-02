#include "TextureFabric.h"
#include "EverydayTools/Exception/CallAndRethrow.h"
#include "Resource/Texture/Texture.h"
#include "Keng/Graphics/GraphicsSystem.h"
#include "yasli/STL.h"
#include "Keng/Base/Serialization/SerializeMandatory.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace keng::graphics
{
    std::string_view TextureFabric::GetNodeName() const {
        return "texture";
    }

    std::string_view TextureFabric::GetResourceType() const {
        return "Texture";
    }

    core::Ptr<resource::IResource> TextureFabric::LoadResource(resource::IResourceSystem*,
        Archive& ar, const core::Ptr<resource::IDevice>& abstractDevice) const {
        return CallAndRethrowM + [&] {
            edt::ThrowIfFailed(abstractDevice != nullptr, "Can't create texture without device");
            auto device = std::dynamic_pointer_cast<Device>(abstractDevice);

            struct TextureInfo {
                void serialize(Archive& ar) {
                    SerializeMandatory(ar, file, "file");
                }
                std::string file;
            };

            TextureInfo info;
            SerializeMandatory(ar, info, GetNodeName().data());

            //std::ifstream textureFile;
            //textureFile.open(textureFilename.data());
            //edt::ThrowIfFailed(textureFile.is_open(), "Could not open file \"", textureFilename, "\"");
            //
            //auto content = std::string(
            //	(std::istreambuf_iterator<char>(textureFile)),
            //	(std::istreambuf_iterator<char>()));
            //
            //int w, h, n;
            //auto img_data = stbi_load_from_memory(
            //	(const stbi_uc*)content.data(),
            //	static_cast<int>(content.size()),
            //	&w, &h, &n, 3);

            int w, h, n;
            std::unique_ptr<stbi_uc[]> img_data;
            img_data.reset(stbi_load(info.file.data(), &w, &h, &n, 4));
            auto ptr = img_data.get();

            return core::Ptr<Texture>::MakeInstance(*device, w, h, FragmentFormat::R8_G8_B8_A8_UNORM, d3d_tools::TextureFlags::ShaderResource, ptr);
        };
    }
}
