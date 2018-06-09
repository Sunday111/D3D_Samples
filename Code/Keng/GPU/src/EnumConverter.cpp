#include "EnumConverter.h"
#include "Keng/GraphicsCommon/FilteringMode.h"
#include "EverydayTools/Exception/CallAndRethrow.h"

namespace keng::graphics::gpu
{
    DXGI_FORMAT ConvertTextureFormat(const FragmentFormat& from) {
        return CallAndRethrowM + [&] {
            switch (from) {
            case FragmentFormat::R8_UNORM: return DXGI_FORMAT_R8_UNORM;
            case FragmentFormat::R8_G8_B8_A8_UNORM: return DXGI_FORMAT_R8G8B8A8_UNORM;
            case FragmentFormat::R24_G8_TYPELESS: return DXGI_FORMAT_R24G8_TYPELESS;
            case FragmentFormat::D24_UNORM_S8_UINT: return DXGI_FORMAT_D24_UNORM_S8_UINT;
            case FragmentFormat::R24_UNORM_X8_TYPELESS: return DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
            default: throw std::runtime_error("This texture format is not implemented here");
            }
        };
    }

    FragmentFormat ConvertTextureFormat(const DXGI_FORMAT& from) {
        return CallAndRethrowM + [&] {
            switch (from) {
            case DXGI_FORMAT_R8_UNORM: return FragmentFormat::R8_UNORM;
            case DXGI_FORMAT_R8G8B8A8_UNORM: return FragmentFormat::R8_G8_B8_A8_UNORM;
            case DXGI_FORMAT_R24G8_TYPELESS: return FragmentFormat::R24_G8_TYPELESS;
            case DXGI_FORMAT_D24_UNORM_S8_UINT: return FragmentFormat::D24_UNORM_S8_UINT;
            case DXGI_FORMAT_R24_UNORM_X8_TYPELESS: return FragmentFormat::R24_UNORM_X8_TYPELESS;
            default: throw std::runtime_error("This texture format is not implemented here");
            }
        };
    }

    D3D11_TEXTURE_ADDRESS_MODE ConvertAddressMode(const TextureAddressMode& from) {
        return CallAndRethrowM + [&] {
            switch (from) {
            case TextureAddressMode::Border: return D3D11_TEXTURE_ADDRESS_BORDER;
            case TextureAddressMode::Clamp: return D3D11_TEXTURE_ADDRESS_CLAMP;
            case TextureAddressMode::Mirror: return D3D11_TEXTURE_ADDRESS_MIRROR;
            case TextureAddressMode::Wrap: return D3D11_TEXTURE_ADDRESS_WRAP;
            default: throw std::runtime_error("This address mode is not implemented here");
            }
        };
    }

    TextureAddressMode ConvertAddressMode(const D3D11_TEXTURE_ADDRESS_MODE& from) {
        return CallAndRethrowM + [&] {
            switch (from) {
            case D3D11_TEXTURE_ADDRESS_BORDER: return  TextureAddressMode::Border;
            case D3D11_TEXTURE_ADDRESS_CLAMP: return  TextureAddressMode::Clamp;
            case D3D11_TEXTURE_ADDRESS_MIRROR: return  TextureAddressMode::Mirror;
            case D3D11_TEXTURE_ADDRESS_WRAP: return  TextureAddressMode::Wrap;
            default: throw std::runtime_error("This address mode is not implemented here");
            }
        };
    }

    D3D11_FILTER ConvertFilteringMode(const FilteringMode& from) {
        return CallAndRethrowM + [&] {
            switch (from) {
            case FilteringMode::Nearest:     return D3D11_FILTER_MIN_MAG_MIP_POINT;
            case FilteringMode::Bilinear:    return D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
            case FilteringMode::Trilinear:   return D3D11_FILTER_MIN_MAG_MIP_LINEAR;
            case FilteringMode::Anisotropic: return D3D11_FILTER_ANISOTROPIC;
            default: throw std::runtime_error("This filtering mode is not implemented here");
            }
        };
    }

    PrimitiveTopology ConvertTopology(const D3D_PRIMITIVE_TOPOLOGY& from) {
        return CallAndRethrowM + [&] {
            switch (from) {
            case D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST: return PrimitiveTopology::TriangleList;
            case D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP: return PrimitiveTopology::TriangleStrip;
            default: throw std::runtime_error("This topology is not implemented here");
            }
        };
    }

    D3D_PRIMITIVE_TOPOLOGY ConvertTopology(const PrimitiveTopology& from) {
        return CallAndRethrowM + [&] {
            switch (from) {
            case PrimitiveTopology::TriangleList: return D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
            case PrimitiveTopology::TriangleStrip: return D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
            default: throw std::runtime_error("This topology is not implemented here");
            }
        };
    }

    CpuAccessFlags ConvertCpuAccessFlags(UINT flags) {
        auto result = CpuAccessFlags::None;
        if (flags & D3D10_CPU_ACCESS_READ) {
            result |= CpuAccessFlags::Read;
        }
        if (flags & D3D10_CPU_ACCESS_WRITE) {
            result |= CpuAccessFlags::Write;
        }
        return result;
    }

    UINT ConvertCpuAccessFlags(const CpuAccessFlags& flags) {
        UINT result = 0;
        if ((flags & CpuAccessFlags::Read) != CpuAccessFlags::None) {
            result |= D3D10_CPU_ACCESS_READ;
        }
        if ((flags & CpuAccessFlags::Write) != CpuAccessFlags::None) {
            result |= D3D10_CPU_ACCESS_WRITE;
        }
        return result;
    }
}
