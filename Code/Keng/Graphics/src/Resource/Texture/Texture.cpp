#include "Keng/Graphics/Resource/Texture.h"
#include "Keng/Graphics/GraphicsSystem.h"
#include "Xml.h"

namespace keng::graphics
{
    void* Texture::GetNativeInterface() const {
        return GetTexture();
    }

    TextureFormat Texture::GetFormat() const {
        return GetTextureFormat();
    }
}
