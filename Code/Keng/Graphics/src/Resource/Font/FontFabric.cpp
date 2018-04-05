#include "FontFabric.h"
#include "EverydayTools/Exception/CallAndRethrow.h"
#include "EverydayTools/Exception/ThrowIfFailed.h"
#include "Resource/Texture/Texture.h"
#include "yasli/STL.h"
#include "Keng/Base/Serialization/SerializeMandatory.h"
#include "Keng/Base/Serialization/ReadFileToBuffer.h"
#include "Keng/Graphics/Resource/TextureParameters.h"
#include "Device.h"
#include "Font.h"

namespace keng::graphics
{
    std::string_view FontFabric::GetNodeName() const {
        return "font";
    }

    std::string_view FontFabric::GetResourceType() const {
        return "Font";
    }

    resource::IResourcePtr FontFabric::LoadResource(resource::IResourceSystem* system,
        Archive& ar, const resource::IDevicePtr& abstractDevice) const {
        return CallAndRethrowM + [&] {
            UnusedVar(abstractDevice);

            class FileInfo
            {
            public:
                void serialize(Archive& ar) {
                    SerializeMandatory(ar, file, "file");
                }
                std::string file;
            };

            FileInfo info;
            SerializeMandatory(ar, info, GetNodeName().data());
            auto buffer = keng::ReadFileToBuffer(info.file);

            return FontPtr::MakeInstance(std::move(buffer), *system);
        };
    }
}
