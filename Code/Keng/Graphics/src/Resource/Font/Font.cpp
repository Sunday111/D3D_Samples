#include "Font.h"
#include "Resource/Font/FreeType/Library.h"
#include "Device.h"
#include "EverydayTools/Exception/ThrowIfFailed.h"
#include "EverydayTools/Exception/CallAndRethrow.h"
#include "Resource/Font/FreeType/Face.h"
#include "GlyphAtlas.h"
#include "Resource/Texture/Texture.h"
#include "Keng/GraphicsAPI/IDeviceBuffer.h"

namespace keng::graphics
{
    Font::Font(std::vector<uint8_t> fileData, free_type::Library& library, resource::IResourceSystem& resourceSystem) {
        CallAndRethrowM + [&] {
            m_resourceSystem = &resourceSystem;
            m_face = library.LoadFace(std::move(fileData));
        };
    }

    Font::~Font() = default;

    AtlasGlyphInfo Font::RequestGlyphInfo(uint32_t unicode, IDevice& device, const GlyphParameters& params) {
        return CallAndRethrowM + [&] {
            class Temp
            {
            public:
                void OnRequest(const AtlasGlyphInfo& info_) {
                    info = info_;
                }

                AtlasGlyphInfo info;
            } temp;

            edt::Delegate<void(const AtlasGlyphInfo&)> delegate;
            delegate.Bind<Temp, &Temp::OnRequest>(&temp);
            edt::DenseArrayView<const uint32_t> unicodes(&unicode, 1);
            RequestGlyphsInfo(unicodes, device, params, delegate);
            return temp.info;
        };
    }

    void Font::RequestGlyphsInfo(edt::DenseArrayView<const uint32_t> unicodes, IDevice& device, const GlyphParameters& params, edt::Delegate<void(const AtlasGlyphInfo&)> delegate) {
        CallAndRethrowM + [&] {
            auto atlas = FindAtlas(device);
            if (atlas == nullptr) {
                GlyphAtlasParameters atlasParams{};
                atlas = GlyphAtlasPtr::MakeInstance(dynamic_cast<Device&>(device), atlasParams, *this);
                m_atlases.push_back(atlas);
            }
            atlas->BeginUpdate();
            for (auto symbol : unicodes) {
                auto glyphInfo = atlas->GetGlyphInfo(symbol, params);
                delegate.Invoke(glyphInfo);
            }
            atlas->EndUpdate();
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
            auto api_device = device.GetApiDevice();
            for (auto& atlas : m_atlases) {
                auto tex = atlas->GetTexture();
                if (api_device == tex->GetApiTexture()->GetDevice()) {
                    return atlas;
                }
            }

            return nullptr;
        };
    }
}
