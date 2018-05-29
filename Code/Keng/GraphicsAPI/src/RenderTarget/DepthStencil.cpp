#include "DepthStencil.h"
#include "Device.h"
#include "Keng/Graphics/RenderTarget/DepthStencilParameters.h"
#include "Resource/Texture/Texture.h"

namespace keng::graphics_api
{
    DepthStencil::DepthStencil(Device& device, const DepthStencilParameters& p) {
        CallAndRethrowM + [&] {
            m_device = &device;
            m_texture = std::dynamic_pointer_cast<Texture>(p.texture);
            m_dsv = m_texture->GetView<ResourceViewType::DepthStencil>(p.format);
        };
    }
    DepthStencil::~DepthStencil() = default;

    void DepthStencil::Clear(DepthStencilClearFlags f, float depth, uint8_t stencil) {
        CallAndRethrowM + [&] {
            UINT flags = 0;

            if ((f & DepthStencilClearFlags::ClearDepth) != DepthStencilClearFlags::None) flags |= D3D11_CLEAR_DEPTH;
            if ((f & DepthStencilClearFlags::ClearStencil) != DepthStencilClearFlags::None) flags |= D3D11_CLEAR_STENCIL;

            if (flags) {
                m_device->GetContext()->ClearDepthStencilView(m_dsv->GetView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, depth, stencil);
            }
        };
    }

    core::Ptr<TextureView<ResourceViewType::DepthStencil>> DepthStencil::GetView() const {
        return m_dsv;
    }
}
