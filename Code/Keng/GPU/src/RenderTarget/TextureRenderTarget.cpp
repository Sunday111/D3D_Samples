#include "Device.h"
#include "Resource/Texture/DeviceTexture.h"
#include "Resource/Texture/TextureView.h"
#include "DepthStencil.h"
#include "EverydayTools/Exception/ThrowIfFailed.h"
#include "Keng/GraphicsCommon/TextureRenderTargetParameters.h"
#include "TextureRenderTarget.h"

namespace keng::graphics::gpu
{
    TextureRenderTarget::TextureRenderTarget(Device& device, const TextureRenderTargetParameters&, ITexture& texture) {
        CallAndRethrowM + [&] {
            m_device = &device;
            m_texture = &dynamic_cast<DeviceTexture&>(texture);

            D3D11_BLEND_DESC desc{};
            desc.RenderTarget[0].BlendEnable = true;
            desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
            desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
            desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
            desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
            desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
            desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
            desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
            WinAPI<char>::ThrowIfError(m_device->GetDevice()->CreateBlendState(&desc, m_blendState.Receive()));
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
            m_device->GetContext()->OMSetBlendState(m_blendState.Get(), 0, 0xFFFFFFFF);
        };
    }

    void TextureRenderTarget::Clear(const float(&flatColor)[4]) {
        CallAndRethrowM + [&] {
            auto rtv = m_texture->GetView<ResourceViewType::RenderTarget>();
            m_device->GetContext()->ClearRenderTargetView(rtv->GetView(), flatColor);
        };
    }

    ITexture& TextureRenderTarget::GetTexture() const {
        return *m_texture;
    }
}
