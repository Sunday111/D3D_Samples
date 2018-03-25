#pragma once

#include "EverydayTools/EnumFlag.h"
#include "EverydayTools/Array/ArrayView.h"
#include "Keng/Core/IRefCountObject.h"
#include "Keng/Core/Ptr.h"

namespace keng::graphics
{
    class Device;
    class DeviceBufferMapper;

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
        None            = 0,
        VertexBuffer    = (1 << 0),
        IndexBuffer     = (1 << 1),
        ConstantBuffer  = (1 << 2),
        ShaderResource  = (1 << 3),
        RenderTarget    = (1 << 5),
        DepthStencil    = (1 << 6),
        UnorderedAccess = (1 << 7),
        Last
    };

    EDT_ENUM_FLAG_OPERATORS(DeviceBufferBindFlags);

    enum class DeviceAccessFlags
    {
        None  = 0,
        Write = (1 << 0),
        Read  = (1 << 1),
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

    class IDeviceBuffer :
        public core::IRefCountObject
    {
    public:
        virtual void SetBufferData(edt::DenseArrayView<const uint8_t> data) = 0;
        virtual edt::DenseArrayView<uint8_t> Map() = 0;
        virtual void Unmap() = 0;
        virtual void MakeMapper(DeviceBufferMapper& mapper) = 0;

        ~IDeviceBuffer() = default;
    };
}
