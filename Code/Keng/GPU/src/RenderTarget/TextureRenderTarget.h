#pragma once

#include "Keng/GPU/RenderTarget/ITextureRenderTarget.h"

namespace keng::graphics::gpu
{
    class TextureRenderTarget :
        public core::RefCountImpl<ITextureRenderTarget>
    {
    public:
        TextureRenderTarget(Device& device, const TextureRenderTargetParameters& params, ITexture& texture);
        ~TextureRenderTarget();
        virtual void AssignToPipeline(const IDepthStencilPtr& depthStencil) override final;
        virtual void Clear(const float(&flatColor)[4]) override final;
        virtual ITexture& GetTexture() const override final;
    private:
        DevicePtr m_device;
        TexturePtr m_texture;
        ComPtr<ID3D11BlendState> m_blendState;
    };
}