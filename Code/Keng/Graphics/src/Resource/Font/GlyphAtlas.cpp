#include "GlyphAtlas.h"
#include "Device.h"
#include "EverydayTools/Exception/CallAndRethrow.h"
#include "Keng/Graphics/Resource/ITexture.h"
#include "Keng/Graphics/Resource/Font/GlyphParameters.h"
#include "Keng/GraphicsCommon/DeviceTextureParameters.h"
#include "Keng/ResourceSystem/IResource.h"
#include "Keng/ResourceSystem/IResourceSystem.h"
#include "Resource/Texture/Texture.h"
#include "Font.h"

#include <fstream>

namespace keng::graphics
{
    GlyphAtlas::GlyphAtlas(Device& device, const GlyphAtlasParameters& params, Font& font) {
        CallAndRethrowM + [&] {
            m_font = &font;
            m_parameters = params;
            auto resourceSystem = font.GetResourceSystem();
            auto textureSize = m_parameters.width * m_parameters.height;
            m_cpuTexture.reset(new uint8_t[textureSize]);
            std::memset(m_cpuTexture.get(), 0, textureSize);

            DeviceTextureParameters textureParameters{};
            textureParameters.width = m_parameters.width;
            textureParameters.height = m_parameters.height;
            textureParameters.bindFlags = DeviceBufferBindFlags::ShaderResource;
            textureParameters.format = FragmentFormat::R8_UNORM;
            textureParameters.cpuAccessFlags = CpuAccessFlags::Write;

            m_texture = device.CreateRuntimeTexture(textureParameters, *font.GetResourceSystem());
        };
    }

    GlyphAtlas::~GlyphAtlas() = default;

    TexturePtr GlyphAtlas::GetTexture() const {
        return m_texture;
    }

    AtlasGlyphInfo GlyphAtlas::GetGlyphInfo(uint32_t unicode, const graphics::GlyphParameters& params) {
        return CallAndRethrowM + [&] {
            assert(GetFlagState<StateFlag::Updating>());

            auto it = std::lower_bound(m_glyphs.begin(), m_glyphs.end(), unicode, [&](const AtlasGlyphData& a, size_t unicode) {
                return a.data.unicode < unicode;
            });

            const AtlasGlyphData* data = nullptr;

            if (it == m_glyphs.end() || it->data.unicode != unicode) {
                // Create new glyph info
                free_type::GlyphParameters glyphParams;
                glyphParams.height_pt = params.size;
                glyphParams.width_pt = params.size;
                glyphParams.x_dpi = params.dpiX;
                glyphParams.y_dpi = params.dpiY;
                glyphParams.unicode = unicode;
                data = &AddGlyph(glyphParams, it);
            } else {
                data = &(*it);
            }

            AtlasGlyphInfo i{};
            static_cast<GlyphInfo&>(i) = data->data;
            i.x = data->x;
            i.y = data->y;
            i.texture = m_texture;
            return i;
        };
    }

    void GlyphAtlas::BeginUpdate() {
        SetFlagState<StateFlag::Updating, true>();
    }

    void GlyphAtlas::EndUpdate() {
        FlushTexture();
        SetFlagState<StateFlag::Updating, false>();
    }

    AtlasGlyphData& GlyphAtlas::AddGlyph(const free_type::GlyphParameters& params, AtlasGlyphsCollection::iterator it) {
        return CallAndRethrowM + [&] () -> AtlasGlyphData& {
            size_t sector_w = params.GetMaxWidthPx();
            size_t sector_h = params.GetMaxHeightPx();

            edt::ThrowIfFailed(
                next_x + sector_w <= m_parameters.width,
                "Atlas is full...");

            AtlasGlyphData glyphData;
            auto& face = *m_font->GetFace();
            glyphData.data = face.RenderGlyph(params);
            glyphData.x = next_x;
            glyphData.y = next_y;

            CopyGlyphData(glyphData);

            it = m_glyphs.insert(it, std::move(glyphData));

            constexpr auto distanceBetweenGlyphs = 5;
            next_x += glyphData.data.width + distanceBetweenGlyphs;
            if (next_x + sector_w > m_parameters.width) {
                if (next_y + sector_h <= m_parameters.height) {
                    next_x = 0;
                    next_y += sector_h;
                }
            }

            return *it;
        };
    }

    void GlyphAtlas::FlushTexture() {
        CallAndRethrowM + [&] {
            if (GetFlagState<StateFlag::NeedsFlush>()) {
                SetFlagState<StateFlag::NeedsFlush, false>();
                m_texture->GetApiTexture()->SetData(edt::DenseArrayView<uint8_t>(m_cpuTexture.get(), m_parameters.width * m_parameters.height));
            }
        };
    }

    void GlyphAtlas::CopyGlyphData(const AtlasGlyphData& g) {
        auto gw = g.data.width;
        auto gh = g.data.height;
        for (size_t y = 0; y < gh; ++y) {
            auto textureRowAddress = m_cpuTexture.get() + (g.y + y) * m_parameters.width + g.x;
            auto glyphRowAddress = g.data.buffer.get() + y * gw;
            std::memcpy(textureRowAddress, glyphRowAddress, gw);
        }

        SetFlagState<StateFlag::NeedsFlush, true>();
    }
}
