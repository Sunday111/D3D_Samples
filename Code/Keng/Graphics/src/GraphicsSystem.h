#pragma once

#include "FwdDecl.h"
#include "Keng/FileSystem/FwdDecl.h"
#include "Keng/Graphics/IGraphicsListener.h"
#include "Keng/GraphicsAPI/PrimitiveTopology.h"
#include "Keng/Graphics/IGraphicsSystem.h"
#include "Keng/GraphicsAPI/IGraphicsAPISystem.h"
#include "Keng/GraphicsAPI/IDevice.h"

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
        virtual graphics_api::IWindowRenderTargetPtr CreateWindowRenderTarget(const graphics_api::WindowRenderTargetParameters& params) override;
        virtual graphics_api::ITextureRenderTargetPtr CreateTextureRenderTarget(const graphics_api::TextureRenderTargetParameters& params) override;
        virtual graphics_api::IDepthStencilPtr CreateDepthStencil(const graphics_api::DepthStencilParameters& params) override;
        virtual graphics_api::IDeviceBufferPtr CreateDeviceBuffer(const graphics_api::DeviceBufferParameters& params, edt::DenseArrayView<const uint8_t> data) override;
        virtual IDevicePtr GetDevice() override;
        virtual graphics_api::ISwapChainPtr CreateSwapChain(const graphics_api::SwapChainParameters& params) override;
        virtual ITexturePtr CreateTexture(const graphics_api::TextureParameters& params) override;
        virtual graphics_api::ISamplerPtr CreateSampler(const graphics_api::SamplerParameters& params) override;
        virtual void SetTopology(graphics_api::PrimitiveTopology topo) override;
        virtual void Draw(size_t vertices, size_t offset) override;
        virtual void SetViewport(const graphics_api::ViewportParameters&) override;
        virtual graphics_api::IAnnotationPtr CreateAnnotation() override;


    private:
        bool m_fullscreen = false;
        core::IApplicationPtr m_app;
        DevicePtr m_device;

        resource::IResourceSystemPtr m_resourceSystem;
        graphics_api::IGraphicsAPISystemPtr m_api;
    };
}
