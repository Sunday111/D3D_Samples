#pragma once

#include "Keng/Graphics/FwdDecl.h"
#include "EverydayTools/EnumFlag.h"
#include "EverydayTools/Array/ArrayView.h"
#include "Keng/Graphics/ShaderType.h"

namespace keng::graphics
{
    class Device;

    enum class DeviceBufferUsage
    {
        Default,
        Immutable,
        Dynamic,
        Staging,
        Last
    };

    enum class DeviceBufferBindFlags
    {
        None = 0,
        VertexBuffer = (1 << 0),
        IndexBuffer = (1 << 1),
        ConstantBuffer = (1 << 2),
        ShaderResource = (1 << 3),
        RenderTarget = (1 << 5),
        DepthStencil = (1 << 6),
        UnorderedAccess = (1 << 7),
        Last
    };

    EDT_ENUM_FLAG_OPERATORS(DeviceBufferBindFlags);

    enum class DeviceAccessFlags
    {
        None = 0,
        Write = (1 << 0),
        Read = (1 << 1),
        Last
    };

    EDT_ENUM_FLAG_OPERATORS(DeviceAccessFlags);

    class DeviceBufferParams
    {
    public:
        size_t size = 0;
        DeviceBufferUsage usage = DeviceBufferUsage::Default;
        DeviceBufferBindFlags bindFlags = DeviceBufferBindFlags::None;
        DeviceAccessFlags accessFlags = DeviceAccessFlags::None;
    };

    class VertexBufferAssignParameters
    {
    public:
        uint32_t slot = 0;
        uint32_t stride = 0;
        uint32_t offset = 0;
    };

    class ConstantBufferAssignParameters
    {
    public:
        uint32_t slot = 0;
        uint32_t stride = 0;
        uint32_t offset = 0;
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
