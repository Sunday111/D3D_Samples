#include "Resource/Texture/Texture.h"

namespace keng::graphics
{
    Texture::Texture(graphics_api::ITexture& impl) {
        m_impl = &impl;
    }

    Texture::~Texture() = default;

    graphics_api::ITexturePtr Texture::GetApiTexture() {
        return m_impl;
    }
}
