#include "Resource/Texture/Texture.h"
#include "Device.h"
#include "D3D_11/EnumConverter.h"
#include "Keng/Graphics/Resource/TextureParameters.h"
#include "EverydayTools/Exception/CheckedCast.h"

namespace keng::graphics
{
    namespace
    {
        template<TextureUsage flag>
        bool FlagIsSet(TextureUsage flags) {
            return (flags & flag) != TextureUsage::None;
        }

        UINT MakeBindFlags(TextureUsage flags) {
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

        D3D11_TEXTURE2D_DESC MakeTextureDescription(size_t w, size_t h, FragmentFormat format, TextureUsage usage, CpuAccessFlags cpuAccess) {
            return CallAndRethrowM + [&] {
                D3D11_TEXTURE2D_DESC d{};
                d.Width = edt::CheckedCast<uint32_t>(w);
                d.Height = edt::CheckedCast<uint32_t>(h);
                d.MipLevels = 1;
                d.ArraySize = 1;
                d.Format = d3d::ConvertTextureFormat(format);
                d.SampleDesc.Count = 1;
                d.Usage = D3D11_USAGE_DEFAULT;
                d.BindFlags = MakeBindFlags(usage);
                d.CPUAccessFlags = d3d::ConvertCpuAccessFlags(cpuAccess);

                if ((cpuAccess & CpuAccessFlags::Write) != CpuAccessFlags::None) {
                    d.Usage = D3D11_USAGE_DYNAMIC;
                }

                return d;
            };
        }

        size_t ComputeBytesPerPixel(FragmentFormat format) {
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

    ComPtr<ID3D11Texture2D> Texture::GetTexture() const {
        return m_texture;
    }

    Texture::Texture(Device& device, const TextureParameters& params) {
        m_device = &device;
        m_params = params;
        auto rawDevice = m_device->GetDevice();

        CallAndRethrowM + [&] {
            auto desc = MakeTextureDescription(params.width, params.height, params.format, params.usage, params.cpuAccessFlags);
            HRESULT hres = S_OK;
            if (params.data) {
                D3D11_SUBRESOURCE_DATA subresource {};
                subresource.pSysMem = params.data;
                subresource.SysMemPitch = edt::CheckedCast<UINT>(params.width * ComputeBytesPerPixel(params.format));
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

    FragmentFormat Texture::GetFormat() const {
        return m_params.format;
    }

    void Texture::AssignToPipeline(ShaderType shaderType, size_t slot) {
        auto srv = GetView<ResourceViewType::ShaderResource>();
        m_device->SetShaderResource(edt::CheckedCast<uint32_t>(slot), (d3d_tools::ShaderType)shaderType, srv->GetView());
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

    size_t Texture::GetWidth() const {
        return m_params.width;
    }

    size_t Texture::GetHeight() const {
        return m_params.height;
    }
}
