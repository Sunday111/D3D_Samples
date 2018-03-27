#include "keng/Graphics/Device.h"
#include "Keng/Graphics/Resource/Texture.h"
#include "Keng/Graphics/Resource/TextureView.h"
#include "DepthStencil.h"
#include "EverydayTools/Exception/ThrowIfFailed.h"
#include "Keng/Graphics/RenderTarget/TextureRenderTargetParameters.h"
#include "TextureRenderTarget.h"

namespace keng::graphics
{
    TextureRenderTarget::TextureRenderTarget(Device& device, const TextureRenderTargetParameters& params) {
        CallAndRethrowM + [&] {
            m_device = &device;
            m_texture = std::dynamic_pointer_cast<Texture>(params.renderTarget);
        };
    }

    TextureRenderTarget::~TextureRenderTarget() = default;

    void TextureRenderTarget::Activate(const core::Ptr<IDepthStencil>& depthStencil) {
        CallAndRethrowM + [&] {
            auto rtv = m_texture->GetView<ResourceViewType::RenderTarget>(m_device->GetDevice(), m_texture->GetFormat());
            core::Ptr<TextureView<ResourceViewType::DepthStencil>> dsv;
            if (depthStencil) {
                auto castedDepthStencil = std::dynamic_pointer_cast<DepthStencil>(depthStencil);
                edt::ThrowIfFailed(castedDepthStencil, "Custom depth stencil implementation is not supported");
                dsv = castedDepthStencil->GetView();
            }
            m_device->SetRenderTarget(*rtv, dsv.Get());
        };
    }

    void TextureRenderTarget::ClearRenderTarget(const float(&flatColor)[4]) {
        CallAndRethrowM + [&] {
            auto rtv = m_texture->GetView<ResourceViewType::RenderTarget>(m_device->GetDevice(), m_texture->GetFormat());
            m_device->GetContext()->ClearRenderTargetView(rtv->GetView(), flatColor);
        };
    }

    core::Ptr<ITextureView> TextureRenderTarget::GetRenderTargetView() {
        return CallAndRethrowM + [&] {
            return m_texture->GetView<ResourceViewType::RenderTarget>(m_device->GetDevice(), m_texture->GetFormat());
        };
    }
}
