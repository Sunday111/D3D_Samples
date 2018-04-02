#include "TextureFabric.h"
#include "EverydayTools/Exception/CallAndRethrow.h"
#include "Resource/Texture/Texture.h"
#include "Keng/Graphics/GraphicsSystem.h"
#include "yasli/STL.h"
#include "Keng/Base/Serialization/SerializeMandatory.h"
#include "Resource/Texture/TextureParameters.h"

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

            TextureParameters params;
            SerializeMandatory(ar, params, GetNodeName().data());

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
            if (params.sourceFile.empty())
            {
                edt::ThrowIfFailed(params.width > 0 && params.height > 0, "Bad asset format (texture source file not specified and size is 0 (or not specified too))");
            }
            else
            {
                int w, h, n;
                img_data.reset(stbi_load(params.sourceFile.data(), &w, &h, &n, 4));
                params.width = static_cast<uint32_t>(w);
                params.height = static_cast<uint32_t>(h);
            }

            return TexturePtr::MakeInstance(
                *device, params.width, params.height,
                FragmentFormat::R8_G8_B8_A8_UNORM,
                d3d_tools::TextureFlags::ShaderResource,
                img_data.get());
        };
    }
}
