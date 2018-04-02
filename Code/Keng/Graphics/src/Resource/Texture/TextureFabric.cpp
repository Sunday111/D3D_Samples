#include "TextureFabric.h"
#include "EverydayTools/Exception/CallAndRethrow.h"
#include "Resource/Texture/Texture.h"
#include "Keng/Graphics/GraphicsSystem.h"
#include "yasli/STL.h"
#include "Keng/Base/Serialization/SerializeMandatory.h"
#include "Keng/Graphics/Resource/TextureParameters.h"

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

    resource::IResourcePtr TextureFabric::LoadResource(resource::IResourceSystem*,
        Archive& ar, const resource::IDevicePtr& abstractDevice) const {
        return CallAndRethrowM + [&] {
            edt::ThrowIfFailed(abstractDevice != nullptr, "Can't create texture without device");
            auto device = std::dynamic_pointer_cast<Device>(abstractDevice);

            class FileInfo {
            public:
                void serialize(Archive& ar) {
                    SerializeMandatory(ar, file, "file");
                }
                std::string file;
            };

            FileInfo info;
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


            std::unique_ptr<stbi_uc[]> img_data;
            int w, h, n;
            img_data.reset(stbi_load(info.file.data(), &w, &h, &n, 4));

            TextureParameters params {};
            params.width = static_cast<uint32_t>(w);
            params.height = static_cast<uint32_t>(h);
            params.usage = TextureUsage::ShaderResource;
            params.format = FragmentFormat::R8_G8_B8_A8_UNORM;
            params.data = img_data.get();

            return TexturePtr::MakeInstance(*device, params);
        };
    }
}
