#include "Resource/Texture/Texture.h"
#include "Device.h"
#include "D3D_11/EnumConverter.h"
#include "Keng/Graphics/Resource/TextureParameters.h"

namespace keng::graphics
{
    namespace
    {
        template<TextureUsage flag>
        static bool FlagIsSet(TextureUsage flags) {
            return (flags & flag) != TextureUsage::None;
        }

        static UINT MakeBindFlags(TextureUsage flags) {
            return CallAndRethrowM + [&] {
                // Check for confilected flags:
                if (FlagIsSet<TextureUsage::RenderTarget>(flags) &&
                    FlagIsSet<TextureUsage::DepthStencil>(flags)) {
                    throw std::runtime_error("Could not be render target and depth stencil at the same time");
                }
                UINT result = 0;
                if (FlagIsSet<TextureUsage::RenderTarget>(flags)) {
                    result |= D3D11_BIND_RENDER_TARGET;
                }
                if (FlagIsSet<TextureUsage::DepthStencil>(flags)) {
                    result |= D3D11_BIND_DEPTH_STENCIL;
                }
                if (FlagIsSet<TextureUsage::ShaderResource>(flags)) {
                    result |= D3D11_BIND_SHADER_RESOURCE;
                }
                return result;
            };
        }

        static D3D11_TEXTURE2D_DESC MakeTextureDescription(uint32_t w, uint32_t h, FragmentFormat format, TextureUsage flags) {
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

        static size_t ComputeBytesPerPixel(FragmentFormat format) {
            return CallAndRethrowM + [&] {
                switch (format) {
                case FragmentFormat::R8_G8_B8_A8_UNORM:
                case FragmentFormat::R24_G8_TYPELESS:
                case FragmentFormat::D24_UNORM_S8_UINT:
                case FragmentFormat::R24_UNORM_X8_TYPELESS:
                    return 4;
                    break;

                case FragmentFormat::R8_UNORM:
                    return 1;
                    break;
                }

                throw std::exception("This format is not supported here");
            };
        }
    }

    DevicePtr Texture::GetDevice() const {
        return m_device;
    }

    Texture::Texture(Device& device, const TextureParameters& params) {
        m_device = &device;
        auto rawDevice = m_device->GetDevice();

        CallAndRethrowM + [&] {
            auto desc = MakeTextureDescription(params.width, params.height, params.format, params.usage);
            HRESULT hres = S_OK;
            if (params.data) {
                D3D11_SUBRESOURCE_DATA subresource {};
                subresource.pSysMem = params.data;
                subresource.SysMemPitch = static_cast<UINT>(params.width * ComputeBytesPerPixel(params.format));
                hres = rawDevice->CreateTexture2D(&desc, &subresource, m_texture.Receive());
            }
            else {
                hres = rawDevice->CreateTexture2D(&desc, nullptr, m_texture.Receive());
            }
            WinAPI<char>::ThrowIfError(hres);
        };
    }

    Texture::Texture(Device& device, ComPtr<ID3D11Texture2D> texture) {
        CallAndRethrowM + [&] {
            edt::ThrowIfFailed(texture != nullptr, "trying to initialize with null handle");
            m_device = &device;
            m_texture = texture;
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

    void Texture::CopyTo(ITexturePtr abstract){
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
