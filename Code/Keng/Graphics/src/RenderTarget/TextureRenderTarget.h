#pragma once

#include "Keng/Graphics/RenderTarget/ITextureRenderTarget.h"

namespace keng::graphics
{
    class Texture;
    class Device;
    class TextureRenderTargetParameters;

    class TextureRenderTarget : public ITextureRenderTarget
    {
    public:
        IMPLEMENT_IREFCOUNT
    public:
        TextureRenderTarget(Device& device, const TextureRenderTargetParameters& params);
        ~TextureRenderTarget();
        virtual void Activate(const core::Ptr<IDepthStencil>& depthStencil) override;
        virtual void ClearRenderTarget(const float(&flatColor)[4]) override;
        virtual core::Ptr<ITextureView> GetRenderTargetView() override;
    private:
        core::Ptr<Device> m_device;
        core::Ptr<Texture> m_texture;
    };
}