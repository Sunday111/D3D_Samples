#include "DepthStencil.h"
#include "Device.h"
#include "Keng/GraphicsCommon/DepthStencilParameters.h"
#include "Resource/Texture/DeviceTexture.h"
#include "EnumConverter.h"

namespace keng::graphics::gpu
{
    DepthStencil::DepthStencil(Device& device, const DepthStencilParameters& parameters, ITexture& texture) {
        CallAndRethrowM + [&] {
            m_device = &device;
            m_texture = &dynamic_cast<DeviceTexture&>(texture);
            m_dsv = m_texture->GetView<ResourceViewType::DepthStencil>(parameters.format);
        };
    }
    DepthStencil::~DepthStencil() = default;

    void DepthStencil::Clear(const DepthStencilClearFlags& f, float depth, uint8_t stencil) {
        CallAndRethrowM + [&] {
            if (auto flags = ConvertDepthStencilClearFlags(f)) {
                m_device->GetContext()->ClearDepthStencilView(m_dsv->GetView(), flags, depth, stencil);
            }
        };
    }

    core::Ptr<TextureView<ResourceViewType::DepthStencil>> DepthStencil::GetView() const {
        return m_dsv;
    }
}
