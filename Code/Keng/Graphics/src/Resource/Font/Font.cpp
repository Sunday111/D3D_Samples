#include "Font.h"
#include "Resource/Font/FreeType/Library.h"
#include "Device.h"
#include "EverydayTools/Exception/ThrowIfFailed.h"
#include "EverydayTools/Exception/CallAndRethrow.h"
#include "Resource/Font/FreeType/Face.h"
#include "GlyphAtlas.h"
#include "Resource/Texture/Texture.h"
#include "Keng/Graphics/IDeviceBuffer.h"

namespace keng::graphics
{
    Font::Font(std::vector<uint8_t> fileData, resource::IResourceSystem& resourceSystem) {
        CallAndRethrowM + [&] {
            m_resourceSystem = &resourceSystem;
            m_face = free_type::Library::Instance().LoadFace(std::move(fileData));
        };
    }

    Font::~Font() = default;

    AtlasGlyphInfo Font::RequestGlyphInfo(uint32_t unicode, IDevice& device, const GlyphParameters& params) {
        return CallAndRethrowM + [&] {
            auto atlas = FindAtlas(device);
            if (atlas == nullptr) {
                GlyphAtlasParameters atlasParams{};
                atlas = GlyphAtlasPtr::MakeInstance(dynamic_cast<Device&>(device), atlasParams, *this);
                m_atlases.push_back(atlas);
            }
            atlas->BeginUpdate();
            auto result = atlas->GetGlyphInfo(unicode, params);
            atlas->EndUpdate();
            return result;
        };
    }

    free_type::FacePtr Font::GetFace() const {
        return m_face;
    }

    resource::IResourceSystemPtr Font::GetResourceSystem() const {
        return m_resourceSystem;
    }

    GlyphAtlasPtr Font::FindAtlas(IDevice& device) const {
        return CallAndRethrowM + [&]() -> GlyphAtlasPtr {
            IDevicePtr devicePtr = &device;
            for (auto& atlas : m_atlases) {
                auto tex = atlas->GetTexture();
                if (devicePtr == tex->GetDevice()) {
                    return atlas;
                }
            }

            return nullptr;
        };
    }
}
