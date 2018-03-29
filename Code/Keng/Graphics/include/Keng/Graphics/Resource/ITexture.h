#pragma once

#include "EverydayTools\Exception\CallAndRethrow.h"
#include "Keng/Graphics/Resource/TextureView.h"
#include "Keng/ResourceSystem/IResource.h"
#include "Keng/Core/Ptr.h"

#include <vector>

#include "Keng/Graphics/FragmentFormat.h"

namespace keng::graphics
{
    using d3d_tools::TextureFlags;

    class Device;

    class ITexture : public resource::IResource
    {
    public:
        virtual void* GetNativeInterface() const = 0;
        virtual FragmentFormat GetFormat() const = 0;
        virtual core::Ptr<ITextureView> GetView(ResourceViewType viewType, FragmentFormat format) = 0;
        virtual ~ITexture() = default;
    };
}
