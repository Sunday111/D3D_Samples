#include "Device.h"
#include "EverydayTools/Exception/CallAndRethrow.h"
#include "Keng/ResourceSystem/IResourceSystem.h"

namespace keng::graphics
{
    Device::Device(graphics_api::IDevice& impl) {
        m_impl = &impl;
    }

    Device::~Device() = default;

    graphics_api::IDevicePtr Device::GetApiDevice() {
        return m_impl;
    }

    TexturePtr Device::CreateTexture(const graphics_api::TextureParameters& parameters) {
        return CallAndRethrowM + [&] {
            auto apitexture = m_impl->CreateTexture(parameters);
            auto texture = TexturePtr::MakeInstance(*apitexture);
            return texture;
        };
    }

    TexturePtr Device::CreateRuntimeTexture(const graphics_api::TextureParameters& parameters, resource::IResourceSystem& resourceSystem) {
        return CallAndRethrowM + [&] {
            auto texture = CreateTexture(parameters);
            IDevicePtr devicePtr = this;
            resourceSystem.AddRuntimeResource(texture, devicePtr);
            return texture;
        };
    }
}