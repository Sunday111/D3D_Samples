#include "Device.h"
#include "Resource/Texture/Texture.h"
#include "Resource/Texture/TextureView.h"
#include "DepthStencil.h"
#include "EverydayTools/Exception/ThrowIfFailed.h"
#include "Keng/GPU/RenderTarget/TextureRenderTargetParameters.h"
#include "TextureRenderTarget.h"

namespace keng::graphics::gpu
{
    TextureRenderTarget::TextureRenderTarget(Device& device, const TextureRenderTargetParameters& params) {
        CallAndRethrowM + [&] {
            m_device = &device;
            m_texture = std::dynamic_pointer_cast<Texture>(params.renderTarget);
        };
    }

    TextureRenderTarget::~TextureRenderTarget() = default;

    void TextureRenderTarget::AssignToPipeline(const IDepthStencilPtr& depthStencil) {
        CallAndRethrowM + [&] {
            auto rtv = m_texture->GetView<ResourceViewType::RenderTarget>();
            core::Ptr<TextureView<ResourceViewType::DepthStencil>> dsv;
            if (depthStencil) {
                auto castedDepthStencil = std::dynamic_pointer_cast<DepthStencil>(depthStencil);
                edt::ThrowIfFailed(castedDepthStencil, "Custom depth stencil implementation is not supported");
                dsv = castedDepthStencil->GetView();
            }
            m_device->SetRenderTarget(*rtv, dsv.Get());
        };
    }

    void TextureRenderTarget::Clear(const float(&flatColor)[4]) {
        CallAndRethrowM + [&] {
            auto rtv = m_texture->GetView<ResourceViewType::RenderTarget>();
            m_device->GetContext()->ClearRenderTargetView(rtv->GetView(), flatColor);
        };
    }

    ITexturePtr TextureRenderTarget::GetTexture() const {
        return m_texture;
    }
}
