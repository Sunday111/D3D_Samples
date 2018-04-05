#include "GlyphAtlas.h"
#include "Device.h"
#include "Keng/Graphics/Resource/TextureParameters.h"
#include "Keng/ResourceSystem/IResourceSystem.h"
#include "Keng/Graphics/Resource/ITexture.h"
#include "Resource/Texture/Texture.h"
#include "Keng/ResourceSystem/IResource.h"
#include "Font.h"

#include <fstream>

namespace keng::graphics
{
    GlyphAtlas::GlyphAtlas(Device& device, const GlyphAtlasParameters& params, Font& font) {
        CallAndRethrowM + [&] {
            m_font = &font;
            m_parameters = params;
            auto resourceSystem = font.GetResourceSystem();

            std::string letters = "AB";

            free_type::GlyphParameters glyphParams;
            glyphParams.height_pt = 20;
            glyphParams.width_pt = 20;
            glyphParams.x_dpi = 300;
            glyphParams.y_dpi = 300;

            auto textureSize = m_parameters.width * m_parameters.height;
            m_cpuTexture.reset(new uint8_t[textureSize]);
            std::memset(m_cpuTexture.get(), 0, textureSize);

            size_t sector_w = glyphParams.GetMaxWidthPx();
            size_t sector_h = glyphParams.GetMaxHeightPx();

            for (auto letter : letters) {
                glyphParams.unicode = letter;
                auto& glyph = AddGlyph(glyphParams);

                size_t gw = glyph.data.width;
                size_t gh = glyph.data.height;
                for (size_t y = 0; y < gh; ++y) {
                    assert(sector_w >= gw);
                    auto textureRowAddress = m_cpuTexture.get() + (glyph.y + y) * m_parameters.width + glyph.x;
                    auto glyphRowAddress = glyph.data.buffer.get() + (glyph.y + y) * gw;
                    std::memcpy(textureRowAddress, glyphRowAddress, gw);

                    //for (size_t x = 0; x < gw; ++x) {
                    //    auto& from = glyph.data.buffer[y * gw + x];
                    //    auto& to = m_cpuTexture[(glyph.y + y) * m_parameters.width + x + glyph.x];
                    //    to.color[0] = from / 255.f;
                    //    to.color[1] = from / 255.f;
                    //    to.color[2] = from / 255.f;
                    //    to.color[3] = 1.f;
                    //}
                }
            }

            {
                std::ofstream file;
                file.open("tesst.txt");
                for (size_t y = 0; y < m_parameters.width; ++y) {
                    for (size_t x = 0; x < m_parameters.height; ++x) {
                        auto val = m_cpuTexture[x + y * m_parameters.width];
                        file.put(val ? '1' : ' ');
                    }
                    file << std::endl;
                }
            }

            TextureParameters texureParameters{};
            texureParameters.width = m_parameters.width;
            texureParameters.height = m_parameters.height;
            texureParameters.usage = TextureUsage::ShaderResource;
            texureParameters.format = FragmentFormat::R8_UNORM;
            texureParameters.data = m_cpuTexture.get();

            m_texture = TexturePtr::MakeInstance(device, texureParameters);
            resourceSystem->AddRuntimeResource(m_texture, &device);
        };
    }

    GlyphAtlas::~GlyphAtlas() = default;

    TexturePtr GlyphAtlas::GetTexture() const {
        return m_texture;
    }

    AtlasGlyphData& GlyphAtlas::AddGlyph(const free_type::GlyphParameters& params) {
        return CallAndRethrowM + [&] () -> AtlasGlyphData& {
            size_t sector_w = params.GetMaxWidthPx();
            size_t sector_h = params.GetMaxHeightPx();

            edt::ThrowIfFailed(next_x + sector_w <= m_parameters.width, "Atlas is full...");

            AtlasGlyphData glyphData;
            auto& face = *m_font->GetFace();
            glyphData.data = face.RenderGlyph(params);
            glyphData.x = next_x;
            glyphData.y = next_y;
            m_glyphs.push_back(std::move(glyphData));

            if (next_x + sector_w > m_parameters.width) {
                if (next_y + sector_h > m_parameters.height) {
                    next_x += sector_w;
                } else {
                    next_x = 0;
                    next_y += sector_h;
                }
            } else {
                next_x += sector_w;
            }

            return m_glyphs.back();
        };
    }
}
