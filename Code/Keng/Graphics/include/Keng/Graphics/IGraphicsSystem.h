#pragma once

#include "Keng/Graphics/FwdDecl.h"
#include "Keng/Core/ISystem.h"
#include "Keng/GraphicsAPI/PrimitiveTopology.h"
#include "EverydayTools/Array/ArrayView.h"

namespace keng::graphics
{
    class IGraphicsSystem : public core::ISystem
    {
    public:
        static const char* SystemName() { return "KengGraphics"; }
        virtual graphics_api::IWindowRenderTargetPtr CreateWindowRenderTarget(const graphics_api::WindowRenderTargetParameters& params) = 0;
        virtual graphics_api::ITextureRenderTargetPtr CreateTextureRenderTarget(const graphics_api::TextureRenderTargetParameters& params) = 0;
        virtual graphics_api::IDepthStencilPtr CreateDepthStencil(const graphics_api::DepthStencilParameters& params) = 0;
        virtual graphics_api::IDeviceBufferPtr CreateDeviceBuffer(const graphics_api::DeviceBufferParameters& params, edt::DenseArrayView<const uint8_t> data) = 0;
        virtual IDevicePtr GetDevice() = 0;
        virtual graphics_api::ISwapChainPtr CreateSwapChain(const graphics_api::SwapChainParameters& params) = 0;
        virtual ITexturePtr CreateTexture(const graphics_api::TextureParameters& params) = 0;
        virtual graphics_api::ISamplerPtr CreateSampler(const graphics_api::SamplerParameters& params) = 0;
        virtual void SetTopology(graphics_api::PrimitiveTopology topo) = 0;
        virtual void Draw(size_t vertices, size_t offset) = 0;
        virtual void SetViewport(const graphics_api::ViewportParameters&) = 0;
        virtual graphics_api::IAnnotationPtr CreateAnnotation() = 0;
    };
}
