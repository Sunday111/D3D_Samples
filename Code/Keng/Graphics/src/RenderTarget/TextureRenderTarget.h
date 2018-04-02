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
        virtual void AssignToPipeline(const IDepthStencilPtr& depthStencil) override;
        virtual void Clear(const float(&flatColor)[4]) override;
        virtual ITexturePtr GetTexture() const override;
    private:
        DevicePtr m_device;
        TexturePtr m_texture;
    };
}