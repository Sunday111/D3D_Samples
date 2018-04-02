#include "Resource/Texture/Texture.h"
#include "Keng/Graphics/GraphicsSystem.h"
#include "Keng/Graphics/Device.h"
#include "D3D_11/EnumConverter.h"

namespace keng::graphics
{
    namespace
    {
        template<d3d_tools::TextureFlags flag>
        static bool FlagIsSet(d3d_tools::TextureFlags flags) {
            return (flags & flag) != d3d_tools::TextureFlags::None;
        }

        static UINT MakeBindFlags(d3d_tools::TextureFlags flags) {
            return CallAndRethrowM + [&] {
                // Check for confilected flags:
                if (FlagIsSet<d3d_tools::TextureFlags::RenderTarget>(flags) &&
                    FlagIsSet<d3d_tools::TextureFlags::DepthStencil>(flags)) {
                    throw std::runtime_error("Could not be render target and depth stencil at the same time");
                }
                UINT result = 0;
                if (FlagIsSet<d3d_tools::TextureFlags::RenderTarget>(flags)) {
                    result |= D3D11_BIND_RENDER_TARGET;
                }
                if (FlagIsSet<d3d_tools::TextureFlags::DepthStencil>(flags)) {
                    result |= D3D11_BIND_DEPTH_STENCIL;
                }
                if (FlagIsSet<d3d_tools::TextureFlags::ShaderResource>(flags)) {
                    result |= D3D11_BIND_SHADER_RESOURCE;
                }
                return result;
            };
        }

        static D3D11_TEXTURE2D_DESC MakeTextureDescription(uint32_t w, uint32_t h, FragmentFormat format, d3d_tools::TextureFlags flags) {
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

    Texture::Texture(Device& device, ComPtr<ID3D11Texture2D> texture) {
        CallAndRethrowM + [&] {
            edt::ThrowIfFailed(texture != nullptr, "trying tu initialize with null handle");
            m_device = &device;
            m_texture = texture;
        };
    }

    Texture::Texture(Device& device, uint32_t w, uint32_t h, FragmentFormat format, d3d_tools::TextureFlags flags, void* initialData) {
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

    void Texture::AssignToPipeline(ShaderType shaderType, size_t slot) {
        auto srv = GetView<ResourceViewType::ShaderResource>();
        m_device->SetShaderResource(static_cast<uint32_t>(slot), (d3d_tools::ShaderType)shaderType, srv->GetView());
    }

    void Texture::CopyTo(core::Ptr<ITexture> abstract){
        CallAndRethrowM + [&] {
            auto to = std::static_pointer_cast<Texture>(abstract);
            CopyTo(to->m_texture);
        };
    }
    void Texture::CopyTo(const ComPtr<ID3D11Texture2D>& to) {
        CallAndRethrowM + [&] {
            m_device->GetContext()->CopyResource(to.Get(), m_texture.Get());
        };
    }
}
