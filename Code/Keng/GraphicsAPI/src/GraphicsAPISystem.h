#pragma once

#include "FwdDecl.h"
#include "Keng/FileSystem/FwdDecl.h"
#include "Keng/GraphicsAPI/PrimitiveTopology.h"
#include "Keng/GraphicsAPI/IGraphicsAPISystem.h"
#include "Device.h"

namespace keng::graphics_api
{
    class GraphicsAPISystem : public core::RefCountImpl<IGraphicsAPISystem>
    {
    public:
        GraphicsAPISystem();
        ~GraphicsAPISystem();

        // ISystem
        virtual const char* GetSystemName() const override;
        virtual bool ForEachDependency(const edt::Delegate<bool(const char*)>& delegate) const override;
        virtual void Initialize(const core::IApplicationPtr& app) override;
        virtual bool Update() override;
        virtual void Shutdown() override;

        // IGraphicsAPISystem
        virtual IWindowRenderTargetPtr CreateWindowRenderTarget(const WindowRenderTargetParameters& params) override;
        virtual ITextureRenderTargetPtr CreateTextureRenderTarget(const TextureRenderTargetParameters& params) override;
        virtual IDepthStencilPtr CreateDepthStencil(const DepthStencilParameters& params) override;
        virtual IDeviceBufferPtr CreateDeviceBuffer(const DeviceBufferParameters& params, edt::DenseArrayView<const uint8_t> data) override;
        virtual IDevicePtr GetDevice() override;
        virtual ISwapChainPtr CreateSwapChain(const SwapChainParameters& params) override;
        virtual ISamplerPtr CreateSampler(const SamplerParameters& params) override;
        virtual void SetTopology(PrimitiveTopology topo) override;
        virtual void Draw(size_t vertices, size_t offset) override;
        virtual void SetViewport(const ViewportParameters&) override;
        virtual IAnnotationPtr CreateAnnotation() override;

    private:
        bool m_fullscreen = false;
        core::IApplicationPtr m_app;
        core::Ptr<Device> m_device;
    };
}
