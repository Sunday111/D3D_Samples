#pragma once

#include "Keng/Graphics/FwdDecl.h"
#include "Keng/Core/ISystem.h"
#include "Keng/GPU/PrimitiveTopology.h"
#include "EverydayTools/Array/ArrayView.h"

namespace keng::graphics
{
    class IGraphicsSystem : public core::ISystem
    {
    public:
        static const char* SystemName() { return "KengGraphics"; }
        virtual gpu::IWindowRenderTargetPtr CreateWindowRenderTarget(const gpu::WindowRenderTargetParameters& params) = 0;
        virtual gpu::ITextureRenderTargetPtr CreateTextureRenderTarget(const gpu::TextureRenderTargetParameters& params) = 0;
        virtual gpu::IDepthStencilPtr CreateDepthStencil(const gpu::DepthStencilParameters& params) = 0;
        virtual IDevicePtr GetDevice() = 0;
        virtual gpu::ISwapChainPtr CreateSwapChain(const gpu::SwapChainParameters& params) = 0;
        virtual ITexturePtr CreateTexture(const gpu::TextureParameters& params) = 0;
        virtual gpu::ISamplerPtr CreateSampler(const gpu::SamplerParameters& params) = 0;
        virtual void Draw(size_t vertices, size_t offset) = 0;
        virtual void SetViewport(const gpu::ViewportParameters&) = 0;
        virtual gpu::IAnnotationPtr CreateAnnotation() = 0;
    };
}
