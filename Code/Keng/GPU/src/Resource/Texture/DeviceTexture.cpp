#include "Resource/Texture/DeviceTexture.h"
#include "Device.h"
#include "EnumConverter.h"
#include "Keng/GraphicsCommon/DeviceTextureParameters.h"
#include "EverydayTools/Exception/CheckedCast.h"

namespace keng::graphics::gpu
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
                d.Format = ConvertTextureFormat(format);
                d.SampleDesc.Count = 1;
                d.Usage = D3D11_USAGE_DEFAULT;
                d.BindFlags = MakeBindFlags(usage);
                d.CPUAccessFlags = ConvertCpuAccessFlags(cpuAccess);

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

    IDevicePtr DeviceTexture::GetDevice() const {
        return GetDevicePrivate();
    }

    DevicePtr DeviceTexture::GetDevicePrivate() const {
        return m_device;
    }

    ComPtr<ID3D11Texture2D> DeviceTexture::GetTexture() const {
        return m_texture;
    }

    DeviceTexture::DeviceTexture(Device& device, const DeviceTextureParameters& params) {
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

    DeviceTexture::DeviceTexture(Device& device, ComPtr<ID3D11Texture2D> texture) {
        CallAndRethrowM + [&] {
            edt::ThrowIfFailed(texture != nullptr, "trying to initialize with null handle");
            m_device = &device;
            m_texture = texture;
        };
    }

    DeviceTexture::~DeviceTexture() = default;

    FragmentFormat DeviceTexture::GetFormat() const {
        return m_params.format;
    }

    void DeviceTexture::AssignToPipeline(const ShaderType& shaderType, size_t slot) {
        auto srv = GetView<ResourceViewType::ShaderResource>();
        m_device->SetShaderResource(edt::CheckedCast<uint32_t>(slot), shaderType, srv->GetView());
    }

    void DeviceTexture::CopyTo(ITexture& abstract) const {
        CallAndRethrowM + [&] {
            auto& casted = static_cast<DeviceTexture&>(abstract);
            CopyTo(*casted.m_texture);
        };
    }

    void DeviceTexture::SetData(edt::DenseArrayView<const uint8_t> data) {
        CallAndRethrowM + [&] {
            auto fragmentsCount = GetWidth() * GetHeight();
            auto bytesCount = fragmentsCount * ComputeBytesPerPixel(m_params.format);
            edt::ThrowIfFailed(bytesCount == data.GetSize());

            D3D11_MAPPED_SUBRESOURCE subres;
            WinAPI<char>::ThrowIfError(m_device->GetContext()->Map(m_texture.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subres));
            memcpy(subres.pData, data.GetData(), bytesCount);
            m_device->GetContext()->Unmap(m_texture.Get(), 0);
        };
    }

    void DeviceTexture::CopyTo(ID3D11Texture2D& to) const {
        CallAndRethrowM + [&] {
            m_device->GetContext()->CopyResource(&to, m_texture.Get());
        };
    }

    size_t DeviceTexture::GetWidth() const {
        return m_params.width;
    }

    size_t DeviceTexture::GetHeight() const {
        return m_params.height;
    }
}
