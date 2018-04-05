#include "Font.h"
#include "Resource/Font/FreeType/Library.h"
#include "Device.h"
#include "EverydayTools/Exception/ThrowIfFailed.h"
#include "EverydayTools/Exception/CallAndRethrow.h"
#include "Resource/Font/FreeType/Face.h"
#include "GlyphAtlas.h"
#include "Resource/Texture/Texture.h"

namespace keng::graphics
{
    Font::Font(std::vector<uint8_t> fileData, resource::IResourceSystem& resourceSystem) {
        CallAndRethrowM + [&] {
            m_resourceSystem = &resourceSystem;
            m_face = free_type::Library::Instance().LoadFace(std::move(fileData));
        };
    }

    Font::~Font() = default;

    void Font::Do(IDevice& device) {
        GlyphAtlasParameters params{};
        m_atlases.push_back(GlyphAtlasPtr::MakeInstance(
            dynamic_cast<Device&>(device), params, *this));
    }

    ITexturePtr Font::GetTexture(IDevice& device) const {
        return CallAndRethrowM + [&] () -> ITexturePtr {
            IDevicePtr devicePtr = &device;
            for (auto& atlas : m_atlases) {
                auto tex = atlas->GetTexture();
                if (devicePtr == tex->GetDevice()) {
                    return tex;
                }
            }

            return nullptr;
        };
    }

    free_type::FacePtr Font::GetFace() const {
        return m_face;
    }

    resource::IResourceSystemPtr Font::GetResourceSystem() const {
        return m_resourceSystem;
    }
}
