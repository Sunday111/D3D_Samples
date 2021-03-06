#pragma once

#include "FwdDecl.h"
#include <algorithm>
#include <vector>
#include "Keng/GPU/Resource/ITexture.h"
#include "Keng/GraphicsCommon/DeviceTextureParameters.h"
#include "Resource/Texture/TextureView.h"

namespace keng::graphics::gpu
{
    class DeviceTexture :
        public core::RefCountImpl<ITexture>
    {
    public:
        DeviceTexture(Device& device, const DeviceTextureParameters& params);
        DeviceTexture(Device& device, ComPtr<ID3D11Texture2D> texture);
        ~DeviceTexture();

        // ITexture
        virtual FragmentFormat GetFormat() const override final;
        virtual void AssignToPipeline(const ShaderType& shaderType, size_t slot) override final;
        virtual void CopyTo(ITexture&) const override final;
        virtual void SetData(edt::DenseArrayView<const uint8_t> data) override final;
        virtual size_t GetWidth() const override final;
        virtual size_t GetHeight() const override final;
        virtual IDevicePtr GetDevice() const override final;

        void CopyTo(ID3D11Texture2D& to) const;
        DevicePtr GetDevicePrivate() const;

        ComPtr<ID3D11Texture2D> GetTexture() const;

        template<ResourceViewType type>
        core::Ptr<TextureView<type>> GetView() {
            return GetView<type>(GetFormat());
        }

        template<ResourceViewType type>
        core::Ptr<TextureView<type>> GetView(FragmentFormat format) {
            return CallAndRethrowM + [&]() {
                auto& views = GetTypedViews<type>();
                auto it = std::lower_bound(views.begin(), views.end(), format,
                    [](auto& view, FragmentFormat format) {
                    return (int)format < (int)view->GetViewFormat();
                });
                if (it != views.end() && (FragmentFormat)(*it)->GetViewFormat() == format) {
                    return *it;
                }

                auto result = core::Ptr<TextureView<type>>::MakeInstance(m_device->GetDevice().Get(), m_texture.Get(), format);
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

        DevicePtr m_device;
        ComPtr<ID3D11Texture2D> m_texture;
        DeviceTextureParameters m_params;
    };
}
