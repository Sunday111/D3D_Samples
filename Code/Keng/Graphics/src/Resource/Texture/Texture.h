#pragma once

#include <algorithm>
#include "Keng/Graphics/Resource/ITexture.h"

namespace keng::graphics
{
    class Device;

    class Texture :
        public core::RefCountImpl<ITexture>
    {
    public:
        Texture(Device& device, ComPtr<ID3D11Texture2D> texture);
        Texture(Device& device, uint32_t w, uint32_t h, FragmentFormat format, TextureFlags flags, void* initialData = nullptr);

        virtual void* GetNativeInterface() const override;
        virtual FragmentFormat GetFormat() const override;
        virtual core::Ptr<ITextureView> GetView(ResourceViewType viewType, FragmentFormat format) override;
        virtual void AssignToPipeline(ShaderType shaderType, size_t slot) override;
        virtual void CopyTo(core::Ptr<ITexture>) override;
        void CopyTo(const ComPtr<ID3D11Texture2D>& to);

        template<ResourceViewType type>
        core::Ptr<TextureView<type>> GetView(ID3D11Device* device, FragmentFormat format) {
            return CallAndRethrowM + [&]() {
                auto& views = GetTypedViews<type>();
                auto it = std::lower_bound(views.begin(), views.end(), format,
                    [](auto& view, FragmentFormat format) {
                    return (int)format < (int)view->GetViewFormat();
                });
                if (it != views.end() && (FragmentFormat)(*it)->GetViewFormat() == format) {
                    return *it;
                }

                auto result = core::Ptr<TextureView<type>>::MakeInstance(device, m_texture.Get(), (d3d_tools::TextureFormat)format);
                views.insert(it, result);
                return result;
            };
        }

    private:
        template<ResourceViewType viewType>
        using TypedViews = std::vector<core::Ptr<TextureView<viewType>>>;

        template<ResourceViewType vt> TypedViews<vt>& GetTypedViews();
        template<> TypedViews<ResourceViewType::RenderTarget>& GetTypedViews() { return m_rtv; }
        template<> TypedViews<ResourceViewType::DepthStencil>& GetTypedViews() { return m_dsv; }
        template<> TypedViews<ResourceViewType::ShaderResource>& GetTypedViews() { return m_srv; }
        template<> TypedViews<ResourceViewType::RandomAccess>& GetTypedViews() { return m_rav; }

        TypedViews<ResourceViewType::RenderTarget>   m_rtv;
        TypedViews<ResourceViewType::DepthStencil>   m_dsv;
        TypedViews<ResourceViewType::ShaderResource> m_srv;
        TypedViews<ResourceViewType::RandomAccess>   m_rav;

        core::Ptr<Device> m_device;
        ComPtr<ID3D11Texture2D> m_texture;
    };
}
