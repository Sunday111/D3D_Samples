#pragma once

#include "FwdDecl.h"
#include "Keng/GPU/RenderTarget/IDepthStencil.h"
#include "Keng/GPU/Resource/ITexture.h"
#include "Resource/Texture/TextureView.h"

namespace keng::graphics::gpu
{
    class DepthStencil :
        public core::RefCountImpl<IDepthStencil>
    {
    public:
        DepthStencil(Device& device, const DepthStencilParameters&, ITexture&);
        ~DepthStencil();
        virtual void Clear(const DepthStencilClearFlags& flags, float depth, uint8_t stencil) override;
        core::Ptr<TextureView<ResourceViewType::DepthStencil>> GetView() const;

    private:
        DevicePtr m_device;
        TexturePtr m_texture;
        core::Ptr<TextureView<ResourceViewType::DepthStencil>> m_dsv;
    };
}

