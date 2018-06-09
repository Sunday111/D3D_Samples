#include "TextureFabric.h"
#include "EverydayTools/Exception/CallAndRethrow.h"
#include "EverydayTools/Exception/CheckedCast.h"
#include "Resource/Texture/Texture.h"
#include "yasli/STL.h"
#include "Keng/Base/Serialization/SerializeMandatory.h"
#include "Keng/GraphicsCommon/DeviceTextureParameters.h"
#include "Device.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace keng::graphics
{
    const char* TextureFabric::GetNodeName() const {
        return "texture";
    }

    const char* TextureFabric::GetResourceType() const {
        return "Texture";
    }

    resource::IResourcePtr TextureFabric::LoadResource(resource::IResourceSystem&,
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
            SerializeMandatory(ar, info, GetNodeName());


            std::unique_ptr<stbi_uc[]> img_data;
            int w, h, n;
            img_data.reset(stbi_load(info.file.data(), &w, &h, &n, 4));

            DeviceTextureParameters params {};
            params.width = edt::CheckedCast<size_t>(w);
            params.height = edt::CheckedCast<size_t>(h);
            params.bindFlags = DeviceBufferBindFlags::ShaderResource;
            params.format = FragmentFormat::R8_G8_B8_A8_UNORM;
            params.data = img_data.get();
            return device->CreateTexture(params);
        };
    }
}
