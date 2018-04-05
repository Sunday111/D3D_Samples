#pragma once

#include "Keng/Graphics/Resource/IFont.h"
#include "Keng/ResourceSystem/IResourceSystem.h"
#include "FwdDecl.h"
#include <string_view>
#include "FreeType/Face.h"
#include <vector>

namespace keng::graphics
{
    class Font : public core::RefCountImpl<IFont>
    {
    public:
        Font(std::vector<uint8_t> fileData, resource::IResourceSystem& resourceSystem);
        ~Font();

        // IFont
        virtual void Do(IDevice& device) override;
        virtual ITexturePtr GetTexture(IDevice& device) const override;

        free_type::FacePtr GetFace() const;
        resource::IResourceSystemPtr GetResourceSystem() const;

    private:
        free_type::FacePtr m_face;
        resource::IResourceSystemPtr m_resourceSystem;
        std::vector<GlyphAtlasPtr> m_atlases;
    };
}
