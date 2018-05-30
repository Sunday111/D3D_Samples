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
        virtual gpu::IDevicePtr GetApiDevice() override;
        // ~IDevice

        TexturePtr CreateTexture(const gpu::TextureParameters& parameters);
        TexturePtr CreateRuntimeTexture(const gpu::TextureParameters& parameters, resource::IResourceSystem& resourceSystem);

        gpu::IDevicePtr m_impl;
    };
}
