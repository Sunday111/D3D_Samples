#pragma once

#include "FwdDecl.h"
#include "Keng/Graphics/IDevice.h"
#include "Keng/GraphicsAPI/IDevice.h"
#include "Resource/Texture/Texture.h"

namespace keng::graphics
{

    class Device :
        public core::RefCountImpl<IDevice>
    {
    public:
        Device(graphics_api::IDevice& impl);
        ~Device();

        // IDevice
        virtual graphics_api::IDevicePtr GetApiDevice() override;
        // ~IDevice

        TexturePtr CreateTexture(const graphics_api::TextureParameters& parameters);
        TexturePtr CreateRuntimeTexture(const graphics_api::TextureParameters& parameters, resource::IResourceSystem& resourceSystem);

        graphics_api::IDevicePtr m_impl;
    };
}
