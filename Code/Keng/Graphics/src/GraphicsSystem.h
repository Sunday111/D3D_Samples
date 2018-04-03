#pragma once

#include "Keng/Graphics/FwdDecl.h"
#include "Keng/Graphics/IGraphicsListener.h"
#include "Keng/Graphics/PrimitiveTopology.h"
#include "Keng/Graphics/IGraphicsSystem.h"
#include "Device.h"

namespace keng::graphics
{
    class GraphicsSystem : public IGraphicsSystem
    {
    public:
        GraphicsSystem();
        ~GraphicsSystem();

        // ISystem
        virtual std::string_view GetSystemName() const override;
        virtual bool ForEachSystemDependency(bool(*pfn)(std::string_view systemName, void* context), void* context) const override;
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
        core::IApplication* m_app = nullptr;
        resource::IResourceSystem* m_resourceSystem = nullptr;
        core::Ptr<Device> m_device;
        std::vector<std::string> m_dependencies;
    };
}
