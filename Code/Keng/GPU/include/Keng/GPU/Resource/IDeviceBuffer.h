#pragma once

#include "Keng/GPU/FwdDecl.h"
#include "EverydayTools/Array/ArrayView.h"
#include "Keng/GraphicsCommon/ShaderType.h"

namespace keng::graphics::gpu
{
    class Device;

    class VertexBufferAssignParameters
    {
    public:
        size_t slot = 0;
        size_t stride = 0;
        size_t offset = 0;
    };

    class ConstantBufferAssignParameters
    {
    public:
        size_t slot = 0;
        size_t stride = 0;
        size_t offset = 0;
        ShaderType shaderType = ShaderType::Vertex;
    };

    class IDeviceBuffer :
        public core::IRefCountObject
    {
    public:
        virtual void SetBufferData(edt::DenseArrayView<const uint8_t> data) = 0;
        virtual edt::DenseArrayView<uint8_t> Map() = 0;
        virtual void Unmap() = 0;
        virtual void MakeMapper(DeviceBufferMapper& mapper) = 0;

        virtual void AssignToPipeline(const VertexBufferAssignParameters& params) = 0;
        virtual void AssignToPipeline(const ConstantBufferAssignParameters& params) = 0;

        //Temporary!!!
        virtual void* GetNativeBuffer() const = 0;

        ~IDeviceBuffer() = default;
    };
}
