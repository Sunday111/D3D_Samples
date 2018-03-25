#include "Keng/Graphics/IDeviceBuffer.h"
#include "Keng/Graphics/DeviceBufferMapper.h"
#include "d3d11.h"
#include "WinWrappers/ComPtr.h"

namespace keng::graphics
{
    class Device;

    class DeviceBuffer : public IDeviceBuffer
    {
        IMPLEMENT_IREFCOUNT
    public:
        DeviceBuffer(Device& device, const DeviceBufferParams& createParams, edt::DenseArrayView<const uint8_t> data = edt::DenseArrayView<const uint8_t>());

        // IDeviceBuffer
        virtual void SetBufferData(edt::DenseArrayView<const uint8_t> data) override;
        virtual edt::DenseArrayView<uint8_t> Map() override;
        virtual void Unmap() override;
        virtual void MakeMapper(DeviceBufferMapper& mapper) override;
        virtual void* GetNativeBuffer() const override;

    protected:
        void CheckSizeCompatibility(size_t size) const;
    private:
        ComPtr<ID3D11DeviceContext> m_context;
        ComPtr<ID3D11Buffer> m_buffer;
        DeviceBufferParams m_params;
    };
}
