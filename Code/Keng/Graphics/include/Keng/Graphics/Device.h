#pragma once

#include "Keng/Graphics/FwdDecl.h"
#include "D3D_Tools/Device.h"
#include "Keng/Core/Ptr.h"
#include "Keng/Core/IRefCountObject.h"
#include "Keng/ResourceSystem/IDevice.h"

namespace keng::graphics
{
    class IDevice : public resource::IDevice
    {
    public:
        virtual void* GetNativeDevice() const = 0;
        virtual void* GetNativeContext() const = 0;
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
    };
}