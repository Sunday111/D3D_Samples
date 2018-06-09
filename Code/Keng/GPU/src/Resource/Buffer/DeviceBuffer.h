#include "FwdDecl.h"
#include "Keng/GPU/Resource/IDeviceBuffer.h"
#include "Keng/GPU/DeviceBufferMapper.h"
#include "d3d11.h"
#include "WinWrappers/ComPtr.h"

namespace keng::graphics::gpu
{
    class DeviceBuffer : public core::RefCountImpl<IDeviceBuffer>
    {
    public:
        DeviceBuffer(Device& device, const DeviceBufferParameters& createParams, edt::DenseArrayView<const uint8_t> data = edt::DenseArrayView<const uint8_t>());

        // IDeviceBuffer
        virtual void SetBufferData(edt::DenseArrayView<const uint8_t> data) override;
        virtual edt::DenseArrayView<uint8_t> Map() override;
        virtual void Unmap() override;
        virtual void MakeMapper(DeviceBufferMapper& mapper) override;
        virtual void* GetNativeBuffer() const override;
        virtual void AssignToPipeline(const VertexBufferAssignParameters& params) override;
        virtual void AssignToPipeline(const ConstantBufferAssignParameters& params) override;

    protected:
        void CheckSizeCompatibility(size_t size) const;
    private:
        DevicePtr m_device;
        ComPtr<ID3D11Buffer> m_buffer;
        DeviceBufferParameters m_params;
    };
}
