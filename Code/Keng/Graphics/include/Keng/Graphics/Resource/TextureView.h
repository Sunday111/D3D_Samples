#pragma once

#include "D3D_Tools/Texture.h"
#include "Keng/Core/IRefCountObject.h"

namespace keng::graphics
{
    using d3d_tools::ResourceViewType;
    using d3d_tools::TextureFormat;

    class ITextureView : public core::IRefCountObject
    {
    public:
        virtual void* GetNativeInterface() const = 0;
        virtual TextureFormat GetFormat() const = 0;
        virtual ResourceViewType GetViewType() const = 0;
        virtual ~ITextureView() = default;
    };

    template<ResourceViewType type>
    class TextureView :
        public ITextureView,
        public d3d_tools::TextureView<type>
    {
        IMPLEMENT_IREFCOUNT
    public:
        using d3d_tools::TextureView<type>::TextureView;

        ResourceViewType GetViewType() const override {
            return type;
        }

        void* GetNativeInterface() const override {
            return this->GetView();
        }

        TextureFormat GetFormat() const override {
            return this->GetViewFormat();
        }
    };
}
