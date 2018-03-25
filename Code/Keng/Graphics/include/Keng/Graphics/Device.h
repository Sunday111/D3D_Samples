#pragma once

#include "D3D_Tools/Device.h"

namespace keng::graphics
{
    class IDevice
    {
    public:
        virtual void* GetNativeDevice() const = 0;
        virtual void* GetNativeContext() const = 0;
        virtual ~IDevice() = default;
    };
    class Device :
        public IDevice,
        public d3d_tools::Device
    {
    public:
        using d3d_tools::Device::Device;

        void* GetNativeDevice() const override {
            return GetDevice();
        }

        void* GetNativeContext() const override {
            return GetContext().Get();
        }
    };
}