#pragma once

#include "Keng/Graphics/RenderTarget/IDepthStencil.h"
#include "Keng/Graphics/Resource/ITexture.h"
#include "Resource/Texture/TextureView.h"

namespace keng::graphics
{
    class Device;
    class Texture;
    class DepthStencilParameters;

    class DepthStencil :
        public core::RefCountImpl<IDepthStencil>
    {
    public:
        DepthStencil(Device& device, const DepthStencilParameters&);
        ~DepthStencil();
        virtual void Clear(DepthStencilClearFlags flags, float depth, uint8_t stencil) override;
        core::Ptr<TextureView<d3d_tools::ResourceViewType::DepthStencil>> GetView() const;

    private:
        core::Ptr<Device> m_device;
        core::Ptr<Texture> m_texture;
        core::Ptr<TextureView<d3d_tools::ResourceViewType::DepthStencil>> m_dsv;
    };
}

