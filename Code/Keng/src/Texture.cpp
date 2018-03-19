#include "Keng/Rendering/Texture.h"
#include "Keng/Systems/GraphicsSystem/GraphicsSystem.h"
#include "stb_image.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace keng
{
	void* Texture::GetNativeInterface() const {
		return GetTexture();
	}

	TextureFormat Texture::GetFormat() const {
		return GetTextureFormat();
	}

	TextureFabric::TextureFabric(IntrusivePtr<Device> device) :
		m_device(device)
	{
	}

	std::string_view TextureFabric::GetNodeName() const {
		return "texture";
	}

	std::string_view TextureFabric::GetResourceType() const
	{
		return "Texture";
	}

	IntrusivePtr<IResource> TextureFabric::LoadResource(IResourceSystem*, IntrusivePtr<IXmlNode> node) const
	{
		return CallAndRethrowM + [&] {
			auto fileNode = node->GetFirstNode("file");
			auto textureFilename = fileNode->GetValue();

			//std::ifstream textureFile;
			//textureFile.open(textureFilename.data());
			//edt::ThrowIfFailed(textureFile.is_open(), "Could not open file \"", textureFilename, "\"");
			//
			//auto content = std::string(
			//	(std::istreambuf_iterator<char>(textureFile)),
			//	(std::istreambuf_iterator<char>()));
			//
			//int w, h, n;
			//auto img_data = stbi_load_from_memory(
			//	(const stbi_uc*)content.data(),
			//	static_cast<int>(content.size()),
			//	&w, &h, &n, 3);

			int w, h, n;
			std::unique_ptr<stbi_uc[]> img_data;
			img_data.reset(stbi_load(textureFilename.data(), &w, &h, &n, 4));
			auto ptr = img_data.get();

			return IntrusivePtr<Texture>::MakeInstance(m_device->GetDevice(), w, h, TextureFormat::R8_G8_B8_A8_UNORM, TextureFlags::ShaderResource, ptr);
		};
	}
}
