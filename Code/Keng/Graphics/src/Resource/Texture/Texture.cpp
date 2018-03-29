#include "Resource/Texture/Texture.h"
#include "Keng/Graphics/GraphicsSystem.h"
#include "Xml.h"
#include "Keng/Graphics/Device.h"
#include "D3D_11/EnumConverter.h"

namespace keng::graphics
{
    namespace
    {
        template<TextureFlags flag>
        static bool FlagIsSet(TextureFlags flags) {
            return (flags & flag) != TextureFlags::None;
        }

        static UINT MakeBindFlags(TextureFlags flags) {
            return CallAndRethrowM + [&] {
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
            };
        }

        static D3D11_TEXTURE2D_DESC MakeTextureDescription(uint32_t w, uint32_t h, FragmentFormat format, TextureFlags flags) {
            return CallAndRethrowM + [&] {
                D3D11_TEXTURE2D_DESC d{};
                d.Width = w;
                d.Height = h;
                d.MipLevels = 1;
                d.ArraySize = 1;
                d.Format = d3d::ConvertTextureFormat(format);
                d.SampleDesc.Count = 1;
                d.Usage = D3D11_USAGE_DEFAULT;
                d.BindFlags = MakeBindFlags(flags);
                return d;
            };
        }

        static size_t ComputeBytesPerPixel(FragmentFormat) {
            // ???
            return 4;
        }
    }

    Texture::Texture(Device& device, uint32_t w, uint32_t h, FragmentFormat format, TextureFlags flags, void* initialData)
    {
        m_device = &device;
        auto rawDevice = m_device->GetDevice();

        CallAndRethrowM + [&] {
            auto desc = MakeTextureDescription(w, h, format, flags);
            HRESULT hres = S_OK;
            if (initialData) {
                D3D11_SUBRESOURCE_DATA subresource{};
                subresource.pSysMem = initialData;
                subresource.SysMemPitch = static_cast<UINT>(w * ComputeBytesPerPixel(format));
                hres = rawDevice->CreateTexture2D(&desc, &subresource, m_texture.Receive());
            } else {
                hres = rawDevice->CreateTexture2D(&desc, nullptr, m_texture.Receive());
            }
            WinAPI<char>::ThrowIfError(hres);
        };
    }

    void* Texture::GetNativeInterface() const {
        return m_texture.Get();
    }

    FragmentFormat Texture::GetFormat() const {
        D3D11_TEXTURE2D_DESC desc;
        m_texture->GetDesc(&desc);
        return d3d::ConvertTextureFormat(desc.Format);
    }

    core::Ptr<ITextureView> Texture::GetView(ResourceViewType viewType, FragmentFormat format) {
        auto device = m_device->GetDevice();
        switch (viewType) {
        case ResourceViewType::RenderTarget: return GetView<ResourceViewType::RenderTarget>(device.Get(), format);
        case ResourceViewType::DepthStencil: return GetView<ResourceViewType::DepthStencil>(device.Get(), format);
        case ResourceViewType::ShaderResource: return GetView<ResourceViewType::ShaderResource>(device.Get(), format);
        case ResourceViewType::RandomAccess: return GetView<ResourceViewType::RandomAccess>(device.Get(), format);
        default:
            break;
        }

        throw std::runtime_error("Unknown resource view type or not implemented");
    }
}
