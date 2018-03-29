#pragma once

#include "D3D_Tools/Device.h"
#include "Keng/Core/Ptr.h"
#include "Keng/Core/IRefCountObject.h"
#include "Keng/Graphics/IDeviceBuffer.h"

namespace keng::graphics
{
    class IDeviceBuffer;

    class IDevice : public core::IRefCountObject
    {
    public:
        virtual void* GetNativeDevice() const = 0;
        virtual void* GetNativeContext() const = 0;
        virtual void SetVertexBuffer(core::Ptr<IDeviceBuffer> buffer, size_t stride, size_t offset) = 0;
        virtual void SetConstantBuffer(core::Ptr<IDeviceBuffer> buffer, d3d_tools::ShaderType shader) = 0;
        virtual ~IDevice() = default;
    };

    class Device :
        public core::RefCountImpl<IDevice>,
        public d3d_tools::Device
    {
    public:
        using d3d_tools::Device::Device;

        void* GetNativeDevice() const override {
            return GetDevice().Get();
        }

        void* GetNativeContext() const override {
            return GetContext().Get();
        }

        virtual void SetVertexBuffer(core::Ptr<IDeviceBuffer> buffer, size_t stride, size_t offset) override {
            d3d_tools::Device::SetVertexBuffer(
                (ID3D11Buffer*)buffer->GetNativeBuffer(),
                static_cast<unsigned>(stride),
                static_cast<unsigned>(offset));
        }

        virtual void SetConstantBuffer(core::Ptr<IDeviceBuffer> buffer, d3d_tools::ShaderType shaderType) override {
            d3d_tools::Device::SetConstantBuffer((ID3D11Buffer*)buffer->GetNativeBuffer(), shaderType);
        }
    };
}