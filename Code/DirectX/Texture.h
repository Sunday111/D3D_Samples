#pragma once

#include "d3d11.h"
#include "EverydayTools\EnumFlag.h"
#include "EverydayTools\Exception\CallAndRethrow.h"
#include "RefCounter.h"
#include "ComPtr.h"

enum class ResourceViewType {
    RenderTarget,
    DepthStencil,
    ShaderResource,
    RandomAccess
};

enum class TextureFormat {
    R8_G8_B8_A8_UNORM,
    R24_G8_TYPELESS,
    D24_UNORM_S8_UINT,
    R24_UNORM_X8_TYPELESS
};

enum class TextureFlags {
    None           = 0,
    RenderTarget   = (1 << 0),
    DepthStencil   = (1 << 1),
    ShaderResource = (1 << 2)
};

EDT_ENUM_FLAG_OPERATORS(TextureFlags);

class ITexture : public IRefCountObject {
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

static DXGI_FORMAT ToDXGI_Format(TextureFormat format) {
    return CallAndRethrow("ToDXGI_Format", [&] {
        switch (format) {
        case TextureFormat::R8_G8_B8_A8_UNORM: return DXGI_FORMAT_R8G8B8A8_UNORM;
        case TextureFormat::R24_G8_TYPELESS: return DXGI_FORMAT_R24G8_TYPELESS;
        case TextureFormat::D24_UNORM_S8_UINT: return DXGI_FORMAT_D24_UNORM_S8_UINT;
        case TextureFormat::R24_UNORM_X8_TYPELESS: return DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
        default: throw std::runtime_error("This texture format is not implemented here");
        }
    });
}

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

    static Description CreateBaseDescription(TextureFormat format) {
        return CallAndRethrow("TextureViewTraitsBase::CreateDescription", [&] {
            Description desc{};
            desc.Format = ToDXGI_Format(format);
            desc.ViewDimension = viewDimension;
            return desc;
        });
    }

    static ComPtr<Interface> MakeInstance(ID3D11Device* device, ID3D11Resource* resource, Description* desc) {
        return CallAndRethrow("TextureViewTraitsBase::MakeInstance", [&] {
            ComPtr<Interface> result;
            auto hresult = (device->*createMethod)(resource, desc, result.Receive());
            WinAPI<char>::ThrowIfError(hresult);
            return result;
        });
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
    static decltype(auto) CreateDescription(TextureFormat format) {
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
    static decltype(auto) CreateDescription(TextureFormat format) {
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
    static decltype(auto) CreateDescription(TextureFormat format) {
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
    static decltype(auto) CreateDescription(TextureFormat format) {
        return CreateBaseDescription(format);
    }
};

template<ResourceViewType type>
decltype(auto) MakeTextureViewDescription(TextureFormat format) {
    using Traits = TextureViewTraits<type>;
    auto result = CreateBaseDescription(format);
    return result;
}

template<ResourceViewType type>
class TextureView : public RefCountImpl<ITextureView>
{
public:
    using Traits = TextureViewTraits<type>;
    using InterfacePtr = ComPtr<typename Traits::Interface>;

    TextureView(InterfacePtr&& ptr) :
        m_view(std::move(ptr))
    {
    }

    TextureView(ID3D11Device* device, ITexture* texture, TextureFormat format) :
        m_format(format)
    {
        CallAndRethrow("TextureView::TextureView", [&] {
            auto nativeTexture = static_cast<ID3D11Texture2D*>(texture->GetNativeInterface());
            auto desc = Traits::CreateDescription(format);
            m_view = Traits::MakeInstance(device, nativeTexture, &desc);
        });
    }

    ResourceViewType GetViewType() const override {
        return type;
    }

    void* GetNativeInterface() const override {
        return m_view.Get();
    }

    TextureFormat GetFormat() const override {
        return m_format;
    }

    decltype(auto) GetView() const {
        return m_view.Get();
    }

    TextureView& operator=(InterfacePtr&& ptr) {
        m_view = std::move(ptr);
        return *this;
    }

private:
    TextureFormat m_format;
    InterfacePtr m_view;
};

class Texture : public RefCountImpl<ITexture>
{
public:
    template<TextureFlags flag>
    static bool FlagIsSet(TextureFlags flags) {
        return (flags & flag) != TextureFlags::None;
    }

    static UINT MakeBindFlags(TextureFlags flags) {
        return CallAndRethrow("Texture::MakeBindFlags", [&] {
            // Check for confilected flags:
            if (FlagIsSet<TextureFlags::RenderTarget>(flags) &&
                FlagIsSet<TextureFlags::DepthStencil>(flags)) {
                throw std::runtime_error("Could not be render target and depth stencil at the same time");
            }
            UINT result = 0;
            if (FlagIsSet<TextureFlags::RenderTarget>(flags)) {
                result |= D3D11_BIND_RENDER_TARGET;
            }
            if (FlagIsSet<TextureFlags::DepthStencil>(flags)) {
                result |= D3D11_BIND_DEPTH_STENCIL;
            }
            if (FlagIsSet<TextureFlags::ShaderResource>(flags)) {
                result |= D3D11_BIND_SHADER_RESOURCE;
            }
            return result;
        });
    }

    static D3D11_TEXTURE2D_DESC MakeTextureDescription(uint32_t w, uint32_t h, TextureFormat format, TextureFlags flags) {
        return CallAndRethrow("Texture::MakeTextureDescription", [&] {
            D3D11_TEXTURE2D_DESC d{};
            d.Width = w;
            d.Height = h;
            d.MipLevels = 1;
            d.ArraySize = 1;
            d.Format = ToDXGI_Format(format);
            d.SampleDesc.Count = 1;
            d.Usage = D3D11_USAGE_DEFAULT;
            d.BindFlags = MakeBindFlags(flags);
            return d;
        });
    }

    Texture(ID3D11Device* device, uint32_t w, uint32_t h, TextureFormat format, TextureFlags flags, void* initialData = nullptr) :
        m_format(format)
    {
        CallAndRethrow("Texture::Texture", [&] {
            auto desc = MakeTextureDescription(w, h, format, flags);
            HRESULT hres = S_OK;
            if (initialData) {
                D3D11_SUBRESOURCE_DATA subresource{};
                subresource.pSysMem = initialData;
                hres = device->CreateTexture2D(&desc, &subresource, m_texture.Receive());
            } else {
                hres = device->CreateTexture2D(&desc, nullptr, m_texture.Receive());
            }
            WinAPI<char>::ThrowIfError(hres);
        });
    }

    template<ResourceViewType type>
    IntrusivePtr<TextureView<type>> GetView(ID3D11Device* device, TextureFormat format) {
        return CallAndRethrow("Texture::GetView", [&] {
            auto result = IntrusivePtr<TextureView<type>>::MakeInstance(device, this, format);
            m_views.push_back(result);
            return result;
        });
    }

    void* GetNativeInterface() const override {
        return m_texture.Get();
    }

    TextureFormat GetFormat() const override {
        return m_format;
    }

private:
    TextureFormat m_format;
    ComPtr<ID3D11Texture2D> m_texture;
    std::vector<IntrusivePtr<ITextureView>> m_views;
};
