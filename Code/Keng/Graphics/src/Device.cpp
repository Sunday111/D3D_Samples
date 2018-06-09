#include "Device.h"
#include "EverydayTools/Exception/CallAndRethrow.h"
#include "Keng/ResourceSystem/IResourceSystem.h"

namespace keng::graphics
{
    Device::Device(gpu::IDevice& impl) {
        m_impl = &impl;
    }

    Device::~Device() = default;

    gpu::IDevicePtr Device::GetApiDevice() {
        return m_impl;
    }

    TexturePtr Device::CreateTexture(const DeviceTextureParameters& parameters) {
        return CallAndRethrowM + [&] {
            auto apitexture = m_impl->CreateTexture(parameters);
            auto texture = TexturePtr::MakeInstance(*apitexture);
            return texture;
        };
    }

    TexturePtr Device::CreateRuntimeTexture(const DeviceTextureParameters& parameters, resource::IResourceSystem& resourceSystem) {
        return CallAndRethrowM + [&] {
            auto texture = CreateTexture(parameters);
            IDevicePtr devicePtr = this;
            resourceSystem.AddRuntimeResource(texture, devicePtr);
            return texture;
        };
    }
}