#pragma once

#include "Keng/GraphicsAPI/FwdDecl.h"
#include "Keng/GraphicsAPI/PrimitiveTopology.h"
#include "Keng/GraphicsAPI/Shader/ShaderType.h"
#include "Keng/Core/ISystem.h"
#include "EverydayTools/Array/ArrayView.h"

namespace keng::graphics_api
{
    class IGraphicsAPISystem : public core::ISystem
    {
    public:
        static const char* SystemName() { return "KengGraphicsAPI"; }
        virtual IWindowRenderTargetPtr CreateWindowRenderTarget(const WindowRenderTargetParameters& params) = 0;
        virtual ITextureRenderTargetPtr CreateTextureRenderTarget(const TextureRenderTargetParameters& params) = 0;
        virtual IDepthStencilPtr CreateDepthStencil(const DepthStencilParameters& params) = 0;
        virtual IDeviceBufferPtr CreateDeviceBuffer(const DeviceBufferParameters& params, edt::DenseArrayView<const uint8_t> data) = 0;
        virtual core::Ptr<IDevice> GetDevice() = 0;
        virtual ISwapChainPtr CreateSwapChain(const SwapChainParameters& params) = 0;
        virtual ISamplerPtr CreateSampler(const SamplerParameters& params) = 0;
        virtual void SetTopology(PrimitiveTopology topo) = 0;
        virtual void Draw(size_t vertices, size_t offset) = 0;
        virtual void SetViewport(const ViewportParameters&) = 0;
        virtual IAnnotationPtr CreateAnnotation() = 0;
    };
}
