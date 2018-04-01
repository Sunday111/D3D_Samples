#pragma once

#include "Keng/Graphics/RenderTarget/ITextureRenderTarget.h"

namespace keng::graphics
{
    class Texture;
    class Device;
    class TextureRenderTargetParameters;

    class TextureRenderTarget :
        public core::RefCountImpl<ITextureRenderTarget>
    {
    public:
        TextureRenderTarget(Device& device, const TextureRenderTargetParameters& params);
        ~TextureRenderTarget();
        virtual void AssignToPipeline(const core::Ptr<IDepthStencil>& depthStencil) override;
        virtual void Clear(const float(&flatColor)[4]) override;
        virtual core::Ptr<ITexture> GetTexture() const override;
    private:
        core::Ptr<Device> m_device;
        core::Ptr<Texture> m_texture;
    };
}