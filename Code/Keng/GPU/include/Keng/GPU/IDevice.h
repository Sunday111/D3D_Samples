#pragma once

#include "Keng/Core/IRefCountObject.h"
#include "Keng/GPU/FwdDecl.h"
#include "EverydayTools/Array/ArrayView.h"

namespace keng::graphics::gpu
{
    class IDevice : public core::IRefCountObject
    {
    public:
        virtual IWindowRenderTargetPtr CreateWindowRenderTarget(const WindowRenderTargetParameters& params, window_system::IWindow& window) = 0;
        virtual ITextureRenderTargetPtr CreateTextureRenderTarget(const TextureRenderTargetParameters&, ITexture&) = 0;
        virtual IDepthStencilPtr CreateDepthStencil(const DepthStencilParameters&, ITexture&) = 0;
        virtual ISwapChainPtr CreateSwapChain(const SwapChainParameters& params, window_system::IWindow& window) = 0;
        virtual ISamplerPtr CreateSampler(const SamplerParameters& params) = 0;
        virtual ITexturePtr CreateTexture(const TextureParameters& params) = 0;
        virtual IShaderPtr CreateShader(const ShaderType& type, const ShaderParameters& parameters) = 0;
        virtual IDeviceBufferPtr CreateDeviceBuffer(const DeviceBufferParameters& params, edt::DenseArrayView<const uint8_t> data) = 0;
        virtual void Draw(size_t vertices, size_t offset) = 0;
        virtual void SetViewport(const ViewportParameters&) = 0;
        virtual IAnnotationPtr CreateAnnotation() = 0;
        virtual void SetTopology(const PrimitiveTopology& topo) = 0;
        virtual void* GetNativeDevice() const = 0;
        virtual void* GetNativeContext() const = 0;
        virtual ~IDevice() = default;
    };
}
