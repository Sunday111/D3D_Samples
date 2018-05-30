#pragma once

#include "FwdDecl.h"
#include "Keng/FileSystem/FwdDecl.h"
#include "Keng/Graphics/IGraphicsListener.h"
#include "Keng/GPU/PrimitiveTopology.h"
#include "Keng/Graphics/IGraphicsSystem.h"
#include "Keng/GPU/IGPUSystem.h"
#include "Keng/GPU/IDevice.h"

namespace keng::graphics
{
    class GraphicsSystem : public core::RefCountImpl<IGraphicsSystem>
    {
    public:
        GraphicsSystem();
        ~GraphicsSystem();

        // ISystem
        virtual const char* GetSystemName() const override;
        virtual bool ForEachDependency(const edt::Delegate<bool(const char*)>& delegate) const override;
        virtual void Initialize(const core::IApplicationPtr& app) override;
        virtual bool Update() override;
        virtual void Shutdown() override;

        // IGraphicsSystem
        virtual gpu::IWindowRenderTargetPtr CreateWindowRenderTarget(const gpu::WindowRenderTargetParameters& params) override;
        virtual gpu::ITextureRenderTargetPtr CreateTextureRenderTarget(const gpu::TextureRenderTargetParameters& params) override;
        virtual gpu::IDepthStencilPtr CreateDepthStencil(const gpu::DepthStencilParameters& params) override;
        virtual IDevicePtr GetDevice() override;
        virtual gpu::ISwapChainPtr CreateSwapChain(const gpu::SwapChainParameters& params) override;
        virtual ITexturePtr CreateTexture(const gpu::TextureParameters& params) override;
        virtual gpu::ISamplerPtr CreateSampler(const gpu::SamplerParameters& params) override;
        virtual void SetTopology(gpu::PrimitiveTopology topo) override;
        virtual void Draw(size_t vertices, size_t offset) override;
        virtual void SetViewport(const gpu::ViewportParameters&) override;
        virtual gpu::IAnnotationPtr CreateAnnotation() override;


    private:
        bool m_fullscreen = false;
        core::IApplicationPtr m_app;
        DevicePtr m_device;

        resource::IResourceSystemPtr m_resourceSystem;
        gpu::IGPUSystemPtr m_api;
    };
}
