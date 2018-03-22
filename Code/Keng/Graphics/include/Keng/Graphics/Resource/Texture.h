#pragma once

#include "d3d11.h"
#include "EverydayTools\EnumFlag.h"
#include "EverydayTools\Exception\CallAndRethrow.h"
#include "WinWrappers/ComPtr.h"
#include "D3D_Tools/Texture.h"
#include "Keng/ResourceSystem/IResource.h"
#include "Keng/ResourceSystem/IResourceFabric.h"

#include <vector>

namespace keng::graphics
{
	using d3d_tools::ResourceViewType;
	using d3d_tools::TextureFormat;
	using d3d_tools::TextureFlags;

	class Device;

	class ITexture : public resource::IResource {
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
            return CallAndRethrowM + [&]() {
                auto& views = GetTypedViews<type>();
                auto it = std::lower_bound(views.begin(), views.end(), format,
                    [](auto& view, TextureFormat format) {
                        return (int)format < (int)view->GetViewFormat();
                    });
                if (it != views.end() && (*it)->GetViewFormat() == format) {
                    return *it;
                }
                auto result = std::make_shared<TextureView<type>>(device, GetTexture(), format);
                views.insert(it, result);
                return result;
            };
		}

    private:
        template<ResourceViewType viewType>
        using TypedViews = std::vector<std::shared_ptr<TextureView<viewType>>>;

        template<ResourceViewType vt> TypedViews<vt>& GetTypedViews();
        template<> TypedViews<ResourceViewType::RenderTarget>& GetTypedViews() { return m_rtv; }
        template<> TypedViews<ResourceViewType::DepthStencil>& GetTypedViews() { return m_dsv; }
        template<> TypedViews<ResourceViewType::ShaderResource>& GetTypedViews() { return m_srv; }
        template<> TypedViews<ResourceViewType::RandomAccess>& GetTypedViews() { return m_rav; }

        TypedViews<ResourceViewType::RenderTarget>   m_rtv;
        TypedViews<ResourceViewType::DepthStencil>   m_dsv;
        TypedViews<ResourceViewType::ShaderResource> m_srv;
        TypedViews<ResourceViewType::RandomAccess>   m_rav;
	};
}
