#pragma once

#include "Keng/Graphics/FwdDecl.h"

#include "Keng/Core/ISystem.h"
#include "Keng/WindowSystem/IWindowListener.h"

#include "Keng/Graphics/Device.h"
#include "Keng/Graphics/Resource/IEffect.h"
#include "IGraphicsListener.h"

#include "EverydayTools/Observable.h"

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

    class GraphicsSystem :
        public IGraphicsSystem,
        public window_system::IWindowListener,
        public Observable<GraphicsSystem, IGraphicsListener>
    {
    public:
        GraphicsSystem();
        ~GraphicsSystem();

        static const char* GetGUID();

        void OnWindowResize(int w, int h) override;

        // ISystem
        virtual bool ForEachSystemDependency(bool(*pfn)(const char* systemGUID, void* context), void* context) override;
        virtual const char* GetSystemGUID() override;
        virtual void Initialize(core::IApplication* app) override;

        // IGraphicsSystem
        virtual IWindowRenderTargetPtr CreateWindowRenderTarget(const WindowRenderTargetParameters& params) override;
        virtual ITextureRenderTargetPtr CreateTextureRenderTarget(const TextureRenderTargetParameters& params) override;
        virtual IDepthStencilPtr CreateDepthStencil(const DepthStencilParameters& params) override;

        core::Ptr<Device> GetDevice() { return m_device; }
        IWindowRenderTargetPtr GetWindowRenderTarget();

        IDeviceBufferPtr CreateDeviceBuffer(const DeviceBufferParams& params, edt::DenseArrayView<const uint8_t> data = edt::DenseArrayView<const uint8_t>());
        ISwapChainPtr CreateSwapChain(const SwapChainParameters& params);

    protected:
        core::Ptr<Device> m_device;
        ITextureRenderTargetPtr m_textureRT;
        IDepthStencilPtr m_depthStencil;

    private:

        bool m_fullscreen = false;
        core::Application* m_app = nullptr;
        std::vector<std::string> m_dependencies;
        IWindowRenderTargetPtr m_windowRT;
    };
}
