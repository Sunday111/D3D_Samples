#include "GlyphAtlas.h"
#include "Device.h"
#include "Keng/Graphics/Resource/TextureParameters.h"
#include "Keng/ResourceSystem/IResourceSystem.h"
#include "Keng/Graphics/Resource/ITexture.h"
#include "Resource/Texture/Texture.h"
#include "Keng/ResourceSystem/IResource.h"
#include "Font.h"
#include "Keng/Graphics/Resource/Font/GlyphParameters.h"

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

            TextureParameters texureParameters{};
            texureParameters.width = m_parameters.width;
            texureParameters.height = m_parameters.height;
            texureParameters.usage = TextureUsage::ShaderResource;
            texureParameters.format = FragmentFormat::R8_UNORM;
            texureParameters.cpuAccessFlags = CpuAccessFlags::Write;

            m_texture = TexturePtr::MakeInstance(device, texureParameters);
            resourceSystem->AddRuntimeResource(m_texture, &device);
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

            next_x += glyphData.data.width;
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
                auto texture = m_texture->GetTexture();
                D3D11_MAPPED_SUBRESOURCE subres;
                WinAPI<char>::ThrowIfError(m_texture->GetDevice()->GetContext()->Map(texture.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subres));
                memcpy(subres.pData, m_cpuTexture.get(), m_parameters.width * m_parameters.height);
                m_texture->GetDevice()->GetContext()->Unmap(texture.Get(), 0);
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
