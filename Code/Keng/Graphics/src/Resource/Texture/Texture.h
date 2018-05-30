#pragma once

#include "FwdDecl.h"
#include "Keng/Graphics/Resource/ITexture.h"
#include "Keng/GraphicsAPI/Resource/ITexture.h"

namespace keng::graphics
{
    class Texture :
        public core::RefCountImpl<ITexture>
    {
    public:
        Texture(graphics_api::ITexture& impl);
        ~Texture();

        virtual graphics_api::ITexturePtr GetApiTexture() override;

        graphics_api::ITexturePtr m_impl;
    };
}
