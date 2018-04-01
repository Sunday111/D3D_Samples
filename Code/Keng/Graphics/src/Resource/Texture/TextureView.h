#pragma once

#include "D3D_Tools/Texture.h"
#include "Keng/Core/IRefCountObject.h"
#include "Keng/Graphics/FragmentFormat.h"

namespace keng::graphics
{
    using d3d_tools::ResourceViewType;

    class ITextureView : public core::IRefCountObject
    {
    public:
        virtual void* GetNativeInterface() const = 0;
        virtual FragmentFormat GetFormat() const = 0;
        virtual ResourceViewType GetViewType() const = 0;
        virtual ~ITextureView() = default;
    };

    template<ResourceViewType type>
    class TextureView :
        public core::RefCountImpl<ITextureView>,
        public d3d_tools::TextureView<type>
    {
    public:
        using d3d_tools::TextureView<type>::TextureView;

        ResourceViewType GetViewType() const override {
            return type;
        }

        void* GetNativeInterface() const override {
            return this->GetView();
        }

        FragmentFormat GetFormat() const override {
            return static_cast<FragmentFormat>(this->GetViewFormat());
        }
    };
}
