#include "Resource/Texture/Texture.h"
#include "Keng/Graphics/GraphicsSystem.h"
#include "Xml.h"
#include "Keng/Graphics/Device.h"

namespace keng::graphics
{
    Texture::Texture(Device& device, uint32_t w, uint32_t h, TextureFormat format, TextureFlags flags, void* initialData) :
        d3d_tools::Texture(device.GetDevice(), w, h, format, flags, initialData)
    {
        m_device = &device;
    }

    void* Texture::GetNativeInterface() const {
        return GetTexture();
    }

    TextureFormat Texture::GetFormat() const {
        return GetTextureFormat();
    }

    core::Ptr<ITextureView> Texture::GetView(ResourceViewType viewType, TextureFormat format) {
        auto device = m_device->GetDevice();
        switch (viewType) {
        case ResourceViewType::RenderTarget: return GetView<ResourceViewType::RenderTarget>(device, format);
        case ResourceViewType::DepthStencil: return GetView<ResourceViewType::DepthStencil>(device, format);
        case ResourceViewType::ShaderResource: return GetView<ResourceViewType::ShaderResource>(device, format);
        case ResourceViewType::RandomAccess: return GetView<ResourceViewType::RandomAccess>(device, format);
        default:
            break;
        }

        throw std::runtime_error("Unknown resource view type or not implemented");
    }
}
