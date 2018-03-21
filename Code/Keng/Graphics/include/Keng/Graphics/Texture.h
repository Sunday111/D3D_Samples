#pragma once

#include "d3d11.h"
#include "EverydayTools\EnumFlag.h"
#include "EverydayTools\Exception\CallAndRethrow.h"
#include "WinWrappers/ComPtr.h"
#include "D3D_Tools/Texture.h"
#include "Keng/Resource/ResourceSystem.h"

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

	class ITextureView : public IRefCountObject {
	public:
		virtual void* GetNativeInterface() const = 0;
		virtual TextureFormat GetFormat() const = 0;
		virtual ResourceViewType GetViewType() const = 0;
		virtual ~ITextureView() = default;
	};

	template<ResourceViewType type>
	class TextureView :
		public RefCountImpl<ITextureView>,
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
		public RefCountImpl<ITexture>,
		public d3d_tools::Texture
	{
	public:
		using d3d_tools::Texture::Texture;

		virtual void* GetNativeInterface() const override;
		virtual TextureFormat GetFormat() const override;

		template<ResourceViewType type>
		IntrusivePtr<TextureView<type>> GetView(ID3D11Device* device, TextureFormat format) {
			return IntrusivePtr<TextureView<type>>::MakeInstance(device, GetTexture(), format);
		}

	};

	class TextureFabric :
		public RefCountImpl<IResourceFabric>
	{
	public:
		TextureFabric(IntrusivePtr<Device>);
		virtual std::string_view GetNodeName() const override;
		virtual std::string_view GetResourceType() const override;
		virtual IntrusivePtr<IResource> LoadResource(IResourceSystem*, IntrusivePtr<IXmlNode> node) const override;
	private:
		IntrusivePtr<Device> m_device;
	};
}
