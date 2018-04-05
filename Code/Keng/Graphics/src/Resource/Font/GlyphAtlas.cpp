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

            std::string letters = "ABCDE";

            free_type::GlyphParameters glyphParams;
            glyphParams.height_pt = 20;
            glyphParams.width_pt = 20;
            glyphParams.x_dpi = 300;
            glyphParams.y_dpi = 300;

            auto textureSize = m_parameters.width * m_parameters.height;
            m_cpuTexture.reset(new uint8_t[textureSize]);
            std::memset(m_cpuTexture.get(), 0, textureSize);

            for (auto letter : letters) {
                glyphParams.unicode = letter;
                AddGlyph(glyphParams);
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

            edt::ThrowIfFailed(
                next_x + sector_w <= m_parameters.width,
                "Atlas is full...");

            AtlasGlyphData glyphData;
            auto& face = *m_font->GetFace();
            glyphData.data = face.RenderGlyph(params);
            glyphData.x = next_x;
            glyphData.y = next_y;

            CopyGlyphData(glyphData);
            m_glyphs.push_back(std::move(glyphData));

            next_x += sector_w;
            if (next_x + sector_w > m_parameters.width) {
                if (next_y + sector_h <= m_parameters.height) {
                    next_x = 0;
                    next_y += sector_h;
                }
            }

            return m_glyphs.back();
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
    }
}
