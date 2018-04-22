#include "FontFabric.h"
#include "EverydayTools/Exception/CallAndRethrow.h"
#include "EverydayTools/Exception/ThrowIfFailed.h"
#include "Resource/Texture/Texture.h"
#include "yasli/STL.h"
#include "Keng/Base/Serialization/SerializeMandatory.h"
#include "Keng/FileSystem/ReadFileToBuffer.h"
#include "Keng/Graphics/Resource/TextureParameters.h"
#include "Device.h"
#include "Resource/Font/Font.h"
#include "Resource/Font/FreeType/Library.h"

namespace keng::graphics
{
    FontFabric::FontFabric() :
        m_library(free_type::LibraryPtr::MakeInstance())
    {
    }

    FontFabric::~FontFabric() = default;

    const char* FontFabric::GetNodeName() const {
        return "font";
    }

    const char* FontFabric::GetResourceType() const {
        return "Font";
    }

    resource::IResourcePtr FontFabric::LoadResource(resource::IResourceSystem& system,
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
            SerializeMandatory(ar, info, GetNodeName());

            using FileView = edt::DenseArrayView<const uint8_t>;
            FontPtr result;
            auto onFileRead = [&](FileView fileView) {
                std::vector<uint8_t> buffer;
                buffer.resize(fileView.GetSize());
                std::memcpy(buffer.data(), fileView.GetData(), fileView.GetSize());
                result = FontPtr::MakeInstance(std::move(buffer), *m_library, system);
            };
            edt::Delegate<void(FileView)> delegate;
            delegate.Bind(onFileRead);
            filesystem::HandleFileData(info.file.data(), delegate);

            return result;
        };
    }
}
