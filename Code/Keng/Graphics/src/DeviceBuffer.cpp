#include "DeviceBuffer.h"
#include "Keng/Graphics/Device.h"
#include "WinWrappers/WinWrappers.h"

namespace keng::graphics
{
    template<typename From, typename To>
    struct EnumFlagConverter
    {
        EnumFlagConverter(From from_) :
            from(from_) {
        }

        bool HasFlag(From flag) const {
            return (from & flag) != From::None;
        }

        void ConvertFlag(From fromFlag, To toFlag) {
            if (HasFlag(fromFlag)) {
                to |= toFlag;
            }
        }

        From from;
        To to = static_cast<To>(0);
    };

    static D3D11_USAGE ConvertD3D(DeviceBufferUsage usage) {
        return CallAndRethrowM + [&] {
            switch (usage) {
            case DeviceBufferUsage::Default: return D3D11_USAGE_DEFAULT;
            case DeviceBufferUsage::Immutable: return D3D11_USAGE_IMMUTABLE;
            case DeviceBufferUsage::Dynamic: return D3D11_USAGE_DYNAMIC;
            case DeviceBufferUsage::Staging: return D3D11_USAGE_STAGING;
            }
            static_assert((size_t)DeviceBufferUsage::Last == 4, "Changed enumeration? Fix here!");
            throw std::runtime_error("Unknown value or not implemented");
        };
    }

    static UINT ConvertD3D(DeviceBufferBindFlags flags) {
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

    static UINT ConvertD3D(DeviceAccessFlags flags) {
        return CallAndRethrowM + [&] {
            EnumFlagConverter<DeviceAccessFlags, UINT> c(flags);
            c.ConvertFlag(DeviceAccessFlags::Write, D3D11_CPU_ACCESS_WRITE);
            c.ConvertFlag(DeviceAccessFlags::Read, D3D11_CPU_ACCESS_READ);
            static_assert((size_t)DeviceAccessFlags::Last == 3, "Changed enumeration? Fix here!");
            return c.to;
        };
    }

    static D3D11_BUFFER_DESC MakeDescription(const DeviceBufferParams& params) {
        return CallAndRethrowM + [&] {
            D3D11_BUFFER_DESC desc{};
            desc.Usage = ConvertD3D(params.usage);
            desc.ByteWidth = static_cast<UINT>(params.size);
            desc.BindFlags = ConvertD3D(params.bindFlags);
            desc.CPUAccessFlags = ConvertD3D(params.accessFlags);
            return desc;
        };
    }

    static ComPtr<ID3D11Buffer> MakeBuffer(const ComPtr<ID3D11Device>& device, const D3D11_BUFFER_DESC& desc, const uint8_t* initialData = nullptr) {
        return CallAndRethrowM + [&] {
            ComPtr<ID3D11Buffer> buffer;
            D3D11_SUBRESOURCE_DATA subresourceData;
            const D3D11_SUBRESOURCE_DATA* pSubresourceData = nullptr;
            if (initialData) {
                subresourceData.pSysMem = initialData;
                subresourceData.SysMemPitch = 0;
                subresourceData.SysMemSlicePitch = 0;
                pSubresourceData = &subresourceData;
            }
            WinAPI<char>::ThrowIfError(device->CreateBuffer(&desc, pSubresourceData, buffer.Receive()));
            return buffer;
        };
    }

    DeviceBuffer::DeviceBuffer(Device& device, const DeviceBufferParams& params, edt::DenseArrayView<const uint8_t> data) :
        m_params(params) {
        CallAndRethrowM + [&] {
            if (data.GetSize() > 0) {
                CheckSizeCompatibility(data.GetSize());
            }
            m_context = device.GetContext();
            m_context.AddReference();
            auto desc = MakeDescription(params);
            m_buffer = MakeBuffer(device.GetDevice(), desc, data.GetData());
        };
    }

    void DeviceBuffer::SetBufferData(edt::DenseArrayView<const uint8_t> data) {
        CallAndRethrowM + [&] {
            CheckSizeCompatibility(data.GetSize());
            DeviceBufferMapper mapper;
            MakeMapper(mapper);
            memcpy(mapper.GetView().GetData(), data.GetData(), data.GetSize());
        };
    }

    edt::DenseArrayView<uint8_t> DeviceBuffer::Map() {
        return CallAndRethrowM + [&] {
            // Map
            D3D11_MAPPED_SUBRESOURCE subresource;
            WinAPI<char>::ThrowIfError(m_context->Map(m_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subresource));
            return edt::DenseArrayView<uint8_t>((uint8_t*)subresource.pData, m_params.size);
        };
    }

    void DeviceBuffer::Unmap() {
        CallAndRethrowM + [&] {
            m_context->Unmap(m_buffer.Get(), 0);
        };
    }

    void DeviceBuffer::CheckSizeCompatibility(size_t size) const {
        edt::ThrowIfFailed(m_params.size == size, "Data size differs from buffer size!");
    }

    void DeviceBuffer::MakeMapper(DeviceBufferMapper& mapper) {
        mapper = DeviceBufferMapper(core::Ptr<DeviceBuffer>(this));
    }

    void* DeviceBuffer::GetNativeBuffer() const {
        return m_buffer.Get();
    }
}
