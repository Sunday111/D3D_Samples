#pragma once

#include "D3D_Tools/Texture.h"
#include "Keng/Core/IRefCountObject.h"
#include "Keng/Graphics/FragmentFormat.h"

namespace keng::graphics
{
    using d3d_tools::ResourceViewType;

    template<ResourceViewType type>
    class TextureView :
        public core::RefCountImpl<core::IRefCountObject>,
        public d3d_tools::TextureView<type>
    {
    public:
        using d3d_tools::TextureView<type>::TextureView;
    };
}
