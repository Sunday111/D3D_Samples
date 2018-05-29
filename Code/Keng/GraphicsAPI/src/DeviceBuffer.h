#include "FwdDecl.h"
#include "Keng/GraphicsAPI/IDeviceBuffer.h"
#include "Keng/GraphicsAPI/DeviceBufferMapper.h"
#include "d3d11.h"
#include "WinWrappers/ComPtr.h"

namespace keng::graphics_api
{
    class DeviceBuffer : public core::RefCountImpl<IDeviceBuffer>
    {
    public:
        DeviceBuffer(Device& device, const DeviceBufferParams& createParams, edt::DenseArrayView<const uint8_t> data = edt::DenseArrayView<const uint8_t>());

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
        DeviceBufferParams m_params;
    };
}
