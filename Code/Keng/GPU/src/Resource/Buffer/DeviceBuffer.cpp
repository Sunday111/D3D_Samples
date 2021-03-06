#include "DeviceBuffer.h"
#include "Device.h"
#include "WinWrappers/WinWrappers.h"
#include "EverydayTools/Exception/CheckedCast.h"
#include "EnumConverter.h"

namespace keng::graphics::gpu
{
    namespace
    {
        D3D11_BUFFER_DESC MakeDescription(const DeviceBufferParameters& params) {
            return CallAndRethrowM + [&] {
                D3D11_BUFFER_DESC desc{};
                desc.Usage = ConvertDeviceBufferUsage(params.usage);
                desc.ByteWidth = edt::CheckedCast<UINT>(params.size);
                desc.BindFlags = ConvertDeviceBufferBindFlags(params.bindFlags);
                desc.CPUAccessFlags = ConvertCpuAccessFlags(params.accessFlags);
                return desc;
            };
        }
        
        ComPtr<ID3D11Buffer> MakeBuffer(const ComPtr<ID3D11Device>& device, const D3D11_BUFFER_DESC& desc, const uint8_t* initialData = nullptr) {
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
    }

    DeviceBuffer::DeviceBuffer(Device& device, const DeviceBufferParameters& params, edt::DenseArrayView<const uint8_t> data) :
        m_params(params) {
        CallAndRethrowM + [&] {
            if (data.GetSize() > 0) {
                CheckSizeCompatibility(data.GetSize());
            }
            m_device = &device;
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
            auto context = m_device->GetContext();
            WinAPI<char>::ThrowIfError(context->Map(m_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subresource));
            return edt::DenseArrayView<uint8_t>((uint8_t*)subresource.pData, m_params.size);
        };
    }

    void DeviceBuffer::Unmap() {
        CallAndRethrowM + [&] {
            m_device->GetContext()->Unmap(m_buffer.Get(), 0);
        };
    }

    void DeviceBuffer::CheckSizeCompatibility(size_t size) const {
        edt::ThrowIfFailed(m_params.size == size, "Data size differs from buffer size!");
    }

    void DeviceBuffer::MakeMapper(DeviceBufferMapper& mapper) {
        mapper = DeviceBufferMapper(DeviceBufferPtr(this));
    }

    void DeviceBuffer::AssignToPipeline(const VertexBufferAssignParameters& params) {
        CallAndRethrowM + [&] {
            edt::ThrowIfFailed(params.stride != 0, "Tring to use buffer as vertex buffer with stride 0!");
            auto rawBuffer = m_buffer.Get();
            auto stride = edt::CheckedCast<uint32_t>(params.stride);
            auto offset = edt::CheckedCast<uint32_t>(params.offset);
            m_device->GetContext()->IASetVertexBuffers(
                edt::CheckedCast<uint32_t>(params.slot), 1, &rawBuffer, &stride, &offset);
        };
    }

    void DeviceBuffer::AssignToPipeline(const ConstantBufferAssignParameters& params) {
        CallAndRethrowM + [&] {
            edt::ThrowIfFailed(params.stride != 0, "Tring to use buffer as constant buffer with stride 0!");

            using Method = void (ID3D11DeviceContext::*)(UINT, UINT, ID3D11Buffer* const *);
            Method method = nullptr;

            switch (params.shaderType) {
            case ShaderType::Compute:
                method = &ID3D11DeviceContext::CSSetConstantBuffers;
                break;
            case ShaderType::Domain:
                method = &ID3D11DeviceContext::DSSetConstantBuffers;
                break;
            case ShaderType::Geometry:
                method = &ID3D11DeviceContext::GSSetConstantBuffers;
                break;
            case ShaderType::Hull:
                method = &ID3D11DeviceContext::HSSetConstantBuffers;
                break;
            case ShaderType::Fragment:
                method = &ID3D11DeviceContext::PSSetConstantBuffers;
                break;
            case ShaderType::Vertex:
                method = &ID3D11DeviceContext::VSSetConstantBuffers;
                break;
            }

            edt::ThrowIfFailed(method != nullptr, "Not implemented for this shader type");
            auto rawBuffer = m_buffer.Get();
            (*m_device->GetContext().*method)(0, 1, &rawBuffer);
        };
    }

    void* DeviceBuffer::GetNativeBuffer() const {
        return m_buffer.Get();
    }
}
