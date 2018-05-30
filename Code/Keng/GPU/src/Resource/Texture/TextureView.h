#pragma once

#include "d3d11_1.h"
#include "Keng/GPU/FragmentFormat.h"
#include "EverydayTools/Exception/CallAndRethrow.h"
#include "EverydayTools/Exception/ThrowIfFailed.h"
#include "WinWrappers/ComPtr.h"
#include "WinWrappers/WinWrappers.h"
#include "D3D_11/EnumConverter.h"

namespace keng::gpu
{
    enum class ResourceViewType
    {
        RenderTarget,
        DepthStencil,
        ShaderResource,
        RandomAccess
    };

    namespace texture_details
    {
        template
        <
            typename InterfaceType,
            typename DescriptionType
        >
        using CreateResourceViewMethodType = HRESULT(ID3D11Device::*)(ID3D11Resource*, const DescriptionType*, InterfaceType**);

        template
        <
            typename InterfaceType,
            typename DescriptionType,
            CreateResourceViewMethodType<InterfaceType, DescriptionType> createMethod,
            decltype(DescriptionType::ViewDimension) viewDimension
        >
        class TextureViewTraitsBase
        {
        public:
            using Interface = InterfaceType;
            using Description = DescriptionType;

            static Description CreateBaseDescription(FragmentFormat format) {
                return CallAndRethrowM + [&] {
                    Description desc{};
                    desc.Format = d3d::ConvertTextureFormat(format);
                    desc.ViewDimension = viewDimension;
                    return desc;
                };
            }

            static Description GetDescription(Interface* iface) {
                Description desc{};
                iface->GetDesc(&desc);
                return desc;
            }

            static ComPtr<Interface> MakeInstance(ID3D11Device* device, ID3D11Resource* resource, Description* desc) {
                return CallAndRethrowM + [&] {
                    ComPtr<Interface> result;
                    auto hresult = (device->*createMethod)(resource, desc, result.Receive());
                    WinAPI<char>::ThrowIfError(hresult);
                    return result;
                };
            }

            static void FillDescSpecific(Description& desc) {
                UnusedVar(desc);
            }
        };

        template<ResourceViewType type>
        struct TextureViewTraits;

        template<>
        class TextureViewTraits<ResourceViewType::RenderTarget> :
            public TextureViewTraitsBase<
            ID3D11RenderTargetView,
            D3D11_RENDER_TARGET_VIEW_DESC,
            &ID3D11Device::CreateRenderTargetView,
            D3D11_RTV_DIMENSION_TEXTURE2D>
        {
        public:
            static decltype(auto) CreateDescription(FragmentFormat format) {
                return CreateBaseDescription(format);
            }
        };

        template<>
        class TextureViewTraits<ResourceViewType::DepthStencil> :
            public TextureViewTraitsBase<
            ID3D11DepthStencilView,
            D3D11_DEPTH_STENCIL_VIEW_DESC,
            &ID3D11Device::CreateDepthStencilView,
            D3D11_DSV_DIMENSION_TEXTURE2D>
        {
        public:
            static decltype(auto) CreateDescription(FragmentFormat format) {
                return CreateBaseDescription(format);
            }
        };

        template<>
        class TextureViewTraits<ResourceViewType::ShaderResource> :
            public TextureViewTraitsBase<
            ID3D11ShaderResourceView,
            D3D11_SHADER_RESOURCE_VIEW_DESC,
            &ID3D11Device::CreateShaderResourceView,
            D3D11_SRV_DIMENSION_TEXTURE2D>
        {
        public:
            static decltype(auto) CreateDescription(FragmentFormat format) {
                auto res = CreateBaseDescription(format);
                res.Texture2D.MipLevels = (UINT)-1;
                res.Texture2D.MostDetailedMip = 0;
                return res;
            }
        };

        template<>
        class TextureViewTraits<ResourceViewType::RandomAccess> :
            public TextureViewTraitsBase<
            ID3D11UnorderedAccessView,
            D3D11_UNORDERED_ACCESS_VIEW_DESC,
            &ID3D11Device::CreateUnorderedAccessView,
            D3D11_UAV_DIMENSION_TEXTURE2D>
        {
        public:
            static decltype(auto) CreateDescription(FragmentFormat format) {
                return CreateBaseDescription(format);
            }
        };

        template<ResourceViewType type>
        decltype(auto) MakeTextureViewDescription(FragmentFormat format) {
            using Traits = TextureViewTraits<type>;
            auto result = CreateBaseDescription(format);
            return result;
        }
    }

    template<ResourceViewType type>
    class TextureView :
        public core::RefCountImpl<core::IRefCountObject>
    {
    public:
        using Traits = texture_details::TextureViewTraits<type>;
        using InterfacePtr = ComPtr<typename Traits::Interface>;

        TextureView(InterfacePtr&& ptr) :
            m_view(std::move(ptr)) {
        }

        TextureView(ID3D11Device* device, ID3D11Texture2D* tex, FragmentFormat format) :
            m_format(format) {
            CallAndRethrowM + [&] {
                auto desc = Traits::CreateDescription(format);
                m_view = Traits::MakeInstance(device, tex, &desc);
            };
        }

        TextureView(ID3D11Device* device, ID3D11Texture2D* tex) {
            CallAndRethrowM + [&] {
                m_view = Traits::MakeInstance(device, tex, nullptr);
                auto desc = Traits::GetDescription(m_view.Get());
                m_format = texture_details::d3d::ConvertTextureFormat(desc.Format);
            };
        }

        decltype(auto) GetView() const {
            return m_view.Get();
        }

        TextureView& operator=(InterfacePtr&& ptr) {
            m_view = std::move(ptr);
            return *this;
        }

        FragmentFormat GetViewFormat() const {
            return m_format;
        }

    private:
        FragmentFormat m_format;
        InterfacePtr m_view;
    };
}
