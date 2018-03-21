#pragma once

#include "d3d11.h"
#include "EverydayTools\EnumFlag.h"
#include "EverydayTools\Exception\CallAndRethrow.h"
#include "WinWrappers/ComPtr.h"
#include "D3D_Tools/Texture.h"
#include "Keng/ResourceSystem/IResource.h"
#include "Keng/ResourceSystem/IResourceFabric.h"

namespace keng
{
	using d3d_tools::ResourceViewType;
	using d3d_tools::TextureFormat;
	using d3d_tools::TextureFlags;

	class Device;

	class ITexture : public IResource {
	public:
		virtual void* GetNativeInterface() const = 0;
		virtual TextureFormat GetFormat() const = 0;
		virtual ~ITexture() = default;
	};

	class ITextureView
    {
	public:
		virtual void* GetNativeInterface() const = 0;
		virtual TextureFormat GetFormat() const = 0;
		virtual ResourceViewType GetViewType() const = 0;
		virtual ~ITextureView() = default;
	};

	template<ResourceViewType type>
	class TextureView :
		public ITextureView,
		public d3d_tools::TextureView<type>
	{
	public:
		using d3d_tools::TextureView<type>::TextureView;

		ResourceViewType GetViewType() const override {
			return type;
		}

		void* GetNativeInterface() const override {
			return this->GetView();
		}

		TextureFormat GetFormat() const override {
			return this->GetViewFormat();
		}
	};

	class Texture :
		public ITexture,
		public d3d_tools::Texture
	{
	public:
		using d3d_tools::Texture::Texture;

		virtual void* GetNativeInterface() const override;
		virtual TextureFormat GetFormat() const override;

		template<ResourceViewType type>
		std::shared_ptr<TextureView<type>> GetView(ID3D11Device* device, TextureFormat format) {
			return std::make_shared<TextureView<type>>(device, GetTexture(), format);
		}

	};

	class TextureFabric :
		public IResourceFabric
	{
	public:
		TextureFabric(std::shared_ptr<Device>);
		virtual std::string_view GetNodeName() const override;
		virtual std::string_view GetResourceType() const override;
		virtual std::shared_ptr<IResource> LoadResource(IResourceSystem*, std::shared_ptr<IXmlNode> node) const override;
	private:
		std::shared_ptr<Device> m_device;
	};
}
