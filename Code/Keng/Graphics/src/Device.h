#pragma once

#include "FwdDecl.h"
#include "Keng/Graphics/IDevice.h"
#include "Keng/GPU/IDevice.h"
#include "Resource/Texture/Texture.h"

namespace keng::graphics
{

    class Device :
        public core::RefCountImpl<IDevice>
    {
    public:
        Device(gpu::IDevice& impl);
        ~Device();

        // IDevice
        virtual gpu::IDevicePtr GetApiDevice() override final;
        // ~IDevice

        TexturePtr CreateTexture(const DeviceTextureParameters& parameters);
        TexturePtr CreateRuntimeTexture(const DeviceTextureParameters& parameters, resource::IResourceSystem& resourceSystem);

        gpu::IDevicePtr m_impl;
    };
}
