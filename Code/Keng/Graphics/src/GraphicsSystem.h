#pragma once

#include "FwdDecl.h"
#include "Keng/FileSystem/FwdDecl.h"
#include "Keng/Graphics/IGraphicsListener.h"
#include "Keng/Graphics/PrimitiveTopology.h"
#include "Keng/Graphics/IGraphicsSystem.h"
#include "Device.h"

namespace keng::graphics
{
    class GraphicsSystem : public core::RefCountImpl<IGraphicsSystem>
    {
    public:
        GraphicsSystem();
        ~GraphicsSystem();

        // ISystem
        virtual std::string_view GetSystemName() const override;
        virtual bool ForEachDependency(const edt::Delegate<bool(std::string_view)>& delegate) const override;
        virtual void Initialize(const core::IApplicationPtr& app) override;
        virtual bool Update() override;
        virtual void Shutdown() override;

        // IGraphicsSystem
        virtual IWindowRenderTargetPtr CreateWindowRenderTarget(const WindowRenderTargetParameters& params) override;
        virtual ITextureRenderTargetPtr CreateTextureRenderTarget(const TextureRenderTargetParameters& params) override;
        virtual IDepthStencilPtr CreateDepthStencil(const DepthStencilParameters& params) override;
        virtual IDeviceBufferPtr CreateDeviceBuffer(const DeviceBufferParams& params, edt::DenseArrayView<const uint8_t> data) override;
        virtual IDevicePtr GetDevice() override;
        virtual ISwapChainPtr CreateSwapChain(const SwapChainParameters& params) override;
        virtual ITexturePtr CreateTexture(const TextureParameters& params) override;
        virtual ISamplerPtr CreateSampler(const SamplerParameters& params) override;
        virtual void SetTopology(PrimitiveTopology topo) override;
        virtual void Draw(size_t vertices, size_t offset) override;
        virtual void SetViewport(const ViewportParameters&) override;
        virtual IAnnotationPtr CreateAnnotation() override;


    private:
        bool m_fullscreen = false;
        core::IApplicationPtr m_app;
        core::Ptr<Device> m_device;

        resource::IResourceSystemPtr m_resourceSystem;
    };
}
