#pragma once

#include "Keng/Graphics/FwdDecl.h"

#include "Keng/Core/ISystem.h"

#include "Keng/Graphics/Device.h"
#include "Keng/Graphics/Resource/IEffect.h"
#include "IGraphicsListener.h"

namespace keng::core
{
    class Application;
}

namespace keng::graphics
{
    class IGraphicsSystem : public core::ISystem
    {
    public:
        virtual IWindowRenderTargetPtr CreateWindowRenderTarget(const WindowRenderTargetParameters& params) = 0;
        virtual ITextureRenderTargetPtr CreateTextureRenderTarget(const TextureRenderTargetParameters& params) = 0;
        virtual IDepthStencilPtr CreateDepthStencil(const DepthStencilParameters& params) = 0;
    };

    class GraphicsSystem : public IGraphicsSystem
    {
    public:
        GraphicsSystem();
        ~GraphicsSystem();

        static const char* GetGUID();

        // ISystem
        virtual bool ForEachSystemDependency(bool(*pfn)(const char* systemGUID, void* context), void* context) override;
        virtual const char* GetSystemGUID() override;
        virtual void Initialize(core::IApplication* app) override;

        // IGraphicsSystem
        virtual IWindowRenderTargetPtr CreateWindowRenderTarget(const WindowRenderTargetParameters& params) override;
        virtual ITextureRenderTargetPtr CreateTextureRenderTarget(const TextureRenderTargetParameters& params) override;
        virtual IDepthStencilPtr CreateDepthStencil(const DepthStencilParameters& params) override;

        core::Ptr<Device> GetDevice() { return m_device; }

        IDeviceBufferPtr CreateDeviceBuffer(const DeviceBufferParams& params, edt::DenseArrayView<const uint8_t> data = edt::DenseArrayView<const uint8_t>());
        ISwapChainPtr CreateSwapChain(const SwapChainParameters& params);
        ITexturePtr CreateTexture(const TextureParameters& params);

    protected:
        core::Ptr<Device> m_device;

    private:
        bool m_fullscreen = false;
        core::Application* m_app = nullptr;
        std::vector<std::string> m_dependencies;
    };
}
