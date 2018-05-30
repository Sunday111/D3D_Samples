#include "Resource/Texture/Texture.h"

namespace keng::graphics
{
    Texture::Texture(gpu::ITexture& impl) {
        m_impl = &impl;
    }

    Texture::~Texture() = default;

    gpu::ITexturePtr Texture::GetApiTexture() {
        return m_impl;
    }
}
