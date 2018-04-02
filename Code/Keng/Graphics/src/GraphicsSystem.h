#pragma once

#include "Keng/Graphics/FwdDecl.h"
#include "Keng/Graphics/IGraphicsListener.h"
#include "Keng/Graphics/PrimitiveTopology.h"
#include "Keng/Graphics/IGraphicsSystem.h"
#include "Device.h"

namespace keng::core
{
    class Application;
}

namespace keng::graphics
{
    class GraphicsSystem : public IGraphicsSystem
    {
    public:
        GraphicsSystem();
        ~GraphicsSystem();

        // ISystem
        virtual bool ForEachSystemDependency(bool(*pfn)(const char* systemGUID, void* context), void* context) override;
        virtual const char* GetSystemGUID() override;
        virtual void Initialize(core::IApplication* app) override;
        virtual bool Update() override;

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
        virtual void Draw(uint32_t vertices, uint32_t offset) override;
        virtual void SetViewport(const ViewportParameters&) override;
        virtual IAnnotationPtr CreateAnnotation() override;


    private:
        bool m_fullscreen = false;
        core::Application* m_app = nullptr;
        core::Ptr<Device> m_device;
        std::vector<std::string> m_dependencies;
    };
}
