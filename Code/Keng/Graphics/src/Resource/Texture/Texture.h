#pragma once

#include "FwdDecl.h"
#include "Keng/Graphics/Resource/ITexture.h"
#include "Keng/GPU/Resource/ITexture.h"

namespace keng::graphics
{
    class Texture :
        public core::RefCountImpl<ITexture>
    {
    public:
        Texture(gpu::ITexture& impl);
        ~Texture();

        virtual gpu::ITexturePtr GetApiTexture() override final;

        gpu::ITexturePtr m_impl;
    };
}
