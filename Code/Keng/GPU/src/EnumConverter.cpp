#include "EnumConverter.h"
#include "Keng/GraphicsCommon/FilteringMode.h"
#include "EverydayTools/Exception/CallAndRethrow.h"

namespace keng::graphics::gpu
{
    namespace
    {
        template<typename From, typename To>
        struct EnumFlagConverter
        {
            EnumFlagConverter(From from_) :
                from(from_) {
            }

            bool HasFlag(From flag) const {
                if constexpr (std::is_enum_v<From>) {
                    return (from & flag) != From::None;
                } else {
                    return (from & flag) != 0;
                }
            }

            void ConvertFlag(From fromFlag, To toFlag) {
                if (HasFlag(fromFlag)) {
                    to |= toFlag;
                }
            }

            From from;
            To to = static_cast<To>(0);
        };
    }

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
            case PrimitiveTopology::LineList:       return D3D10_PRIMITIVE_TOPOLOGY_LINELIST;
            case PrimitiveTopology::LineStrip:      return D3D10_PRIMITIVE_TOPOLOGY_LINESTRIP;
            case PrimitiveTopology::TriangleList:   return D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
            case PrimitiveTopology::TriangleStrip:  return D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
            default: throw std::runtime_error("This topology is not implemented here");
            }
        };
    }

    CpuAccessFlags ConvertCpuAccessFlags(UINT flags) {
        return CallAndRethrowM + [&] {
            EnumFlagConverter<UINT, CpuAccessFlags> c(flags);
            c.ConvertFlag(D3D11_CPU_ACCESS_WRITE, CpuAccessFlags::Write);
            c.ConvertFlag(D3D11_CPU_ACCESS_READ, CpuAccessFlags::Read);
            static_assert((size_t)CpuAccessFlags::Last == 4, "Changed enumeration? Fix here!");
            return c.to;
        };
    }

    UINT ConvertCpuAccessFlags(const CpuAccessFlags& flags) {
        return CallAndRethrowM + [&] {
            EnumFlagConverter<CpuAccessFlags, UINT> c(flags);
            c.ConvertFlag(CpuAccessFlags::Write, D3D11_CPU_ACCESS_WRITE);
            c.ConvertFlag(CpuAccessFlags::Read, D3D11_CPU_ACCESS_READ);
            static_assert((size_t)CpuAccessFlags::Last == 4, "Changed enumeration? Fix here!");
            return c.to;
        };
    }

    UINT ConvertDeviceBufferBindFlags(const DeviceBufferBindFlags& flags) {
        return CallAndRethrowM + [&] {
            EnumFlagConverter<DeviceBufferBindFlags, UINT> c(flags);
            c.ConvertFlag(DeviceBufferBindFlags::VertexBuffer, D3D11_BIND_VERTEX_BUFFER);
            c.ConvertFlag(DeviceBufferBindFlags::IndexBuffer, D3D11_BIND_INDEX_BUFFER);
            c.ConvertFlag(DeviceBufferBindFlags::ConstantBuffer, D3D11_BIND_CONSTANT_BUFFER);
            c.ConvertFlag(DeviceBufferBindFlags::ShaderResource, D3D11_BIND_SHADER_RESOURCE);
            c.ConvertFlag(DeviceBufferBindFlags::RenderTarget, D3D11_BIND_RENDER_TARGET);
            c.ConvertFlag(DeviceBufferBindFlags::DepthStencil, D3D11_BIND_DEPTH_STENCIL);
            c.ConvertFlag(DeviceBufferBindFlags::UnorderedAccess, D3D11_BIND_UNORDERED_ACCESS);
            static_assert((size_t)DeviceBufferBindFlags::Last == 129, "Changed enumeration? Fix here!");
            return c.to;
        };
    }

    DeviceBufferBindFlags ConvertDeviceBufferBindFlags(UINT flags) {
        return CallAndRethrowM + [&] {
            EnumFlagConverter<UINT, DeviceBufferBindFlags> c(flags);
            c.ConvertFlag(D3D11_BIND_VERTEX_BUFFER, DeviceBufferBindFlags::VertexBuffer);
            c.ConvertFlag(D3D11_BIND_INDEX_BUFFER, DeviceBufferBindFlags::IndexBuffer);
            c.ConvertFlag(D3D11_BIND_CONSTANT_BUFFER, DeviceBufferBindFlags::ConstantBuffer);
            c.ConvertFlag(D3D11_BIND_SHADER_RESOURCE, DeviceBufferBindFlags::ShaderResource);
            c.ConvertFlag(D3D11_BIND_RENDER_TARGET, DeviceBufferBindFlags::RenderTarget);
            c.ConvertFlag(D3D11_BIND_DEPTH_STENCIL, DeviceBufferBindFlags::DepthStencil);
            c.ConvertFlag(D3D11_BIND_UNORDERED_ACCESS, DeviceBufferBindFlags::UnorderedAccess);
            static_assert((size_t)DeviceBufferBindFlags::Last == 129, "Changed enumeration? Fix here!");
            return c.to;
        };
    }

    D3D11_USAGE ConvertDeviceBufferUsage(const DeviceBufferUsage& flags) {
        return CallAndRethrowM + [&] {
            switch (flags) {
            case DeviceBufferUsage::Default: return D3D11_USAGE_DEFAULT;
            case DeviceBufferUsage::Immutable: return D3D11_USAGE_IMMUTABLE;
            case DeviceBufferUsage::Dynamic: return D3D11_USAGE_DYNAMIC;
            case DeviceBufferUsage::Staging: return D3D11_USAGE_STAGING;
            }
            static_assert((size_t)DeviceBufferUsage::Last == 4, "Changed enumeration? Fix here!");
            throw std::runtime_error("Unknown value or not implemented");
        };
    }

    DeviceBufferUsage ConvertDeviceBufferUsage(D3D11_USAGE flags) {
        return CallAndRethrowM + [&] {
            switch (flags) {
            case D3D11_USAGE_DEFAULT: return DeviceBufferUsage::Default;
            case D3D11_USAGE_IMMUTABLE: return DeviceBufferUsage::Immutable;
            case D3D11_USAGE_DYNAMIC: return DeviceBufferUsage::Dynamic;
            case D3D11_USAGE_STAGING: return DeviceBufferUsage::Staging;
            }
            static_assert((size_t)DeviceBufferUsage::Last == 4, "Changed enumeration? Fix here!");
            throw std::runtime_error("Unknown value or not implemented");
        };
    }

    UINT ConvertDepthStencilClearFlags(const DepthStencilClearFlags& flags) {
        return CallAndRethrowM + [&] {
            EnumFlagConverter<DepthStencilClearFlags, UINT> c(flags);
            c.ConvertFlag(DepthStencilClearFlags::ClearDepth, D3D11_CLEAR_DEPTH);
            c.ConvertFlag(DepthStencilClearFlags::ClearStencil, D3D11_CLEAR_STENCIL);
            static_assert((size_t)DepthStencilClearFlags::Last == 4, "Changed enumeration? Fix here!");
            return c.to;
        };
    }

    DepthStencilClearFlags ConvertDepthStencilClearFlags(UINT flags) {
        return CallAndRethrowM + [&] {
            EnumFlagConverter<UINT, DepthStencilClearFlags> c(flags);
            c.ConvertFlag(D3D11_CLEAR_DEPTH, DepthStencilClearFlags::ClearDepth);
            c.ConvertFlag(D3D11_CLEAR_STENCIL, DepthStencilClearFlags::ClearStencil);
            static_assert((size_t)DepthStencilClearFlags::Last == 4, "Changed enumeration? Fix here!");
            return c.to;
        };
    }
}
