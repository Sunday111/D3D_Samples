#pragma once

#include "d3d11.h"
#include "EverydayTools\EnumFlag.h"
#include "EverydayTools\Exception\CallAndRethrow.h"
#include "RefCounter.h"
#include "WinWrappers\ComPtr.h"

#include "D3D_Tools\Texture.h"

using d3d_tools::ResourceViewType;
using d3d_tools::TextureFormat;
using d3d_tools::TextureFlags;

class ITexture : public IRefCountObject {
public:
    virtual void* GetNativeInterface() const = 0;
    virtual TextureFormat GetFormat() const = 0;
    virtual ~ITexture() = default;
};

class ITextureView : public IRefCountObject {
public:
    virtual void* GetNativeInterface() const = 0;
    virtual TextureFormat GetFormat() const = 0;
    virtual ResourceViewType GetViewType() const = 0;
    virtual ~ITextureView() = default;
};

template<ResourceViewType type>
class TextureView :
    public RefCountImpl<ITextureView>,
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

    TextureFormat GetFormat() const override {
        return this->GetViewFormat();
    }
};

class Texture :
    public RefCountImpl<ITexture>,
    public d3d_tools::Texture
{
public:
    using d3d_tools::Texture::Texture;

    void* GetNativeInterface() const override {
        return GetTexture();
    }

    TextureFormat GetFormat() const override {
        return GetTextureFormat();
    }

    template<ResourceViewType type>
    IntrusivePtr<TextureView<type>> GetView(ID3D11Device* device, TextureFormat format) {
        return IntrusivePtr<TextureView<type>>::MakeInstance(device, GetTexture(), format);
    }
};
