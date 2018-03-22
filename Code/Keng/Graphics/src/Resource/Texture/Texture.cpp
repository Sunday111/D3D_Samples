#include "Keng/Graphics/Resource/Texture.h"
#include "Keng/Graphics/GraphicsSystem.h"
#include "Xml.h"
#include "stb_image.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace keng::graphics
{
	void* Texture::GetNativeInterface() const {
		return GetTexture();
	}

	TextureFormat Texture::GetFormat() const {
		return GetTextureFormat();
	}
}
