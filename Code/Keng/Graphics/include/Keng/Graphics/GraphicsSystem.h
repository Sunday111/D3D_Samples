#pragma once

#include "Keng/Core/ISystem.h"
#include "Keng/WindowSystem/IWindowListener.h"

#include "Keng/Graphics/Device.h"
#include "Keng/Graphics/ISwapChain.h"
#include "Keng/Graphics/Resource/Texture.h"
#include "Keng/Graphics/Resource/IEffect.h"
#include "IGraphicsListener.h"

#include "D3D_Tools/Texture.h"
#include "D3D_Tools/Shader.h"

#include "EverydayTools/Observable.h"

namespace keng::core
{
    class Application;
}

namespace keng::window_system
{
    class IWindow;
}

namespace keng::graphics
{
    class IDeviceBuffer;
    class DeviceBufferParams;
    class IWindowRenderTarget;
    class WindowRenderTargetParameters;

    class IGraphicsSystem :
        public core::ISystem
    {
    public:
        virtual core::Ptr<IWindowRenderTarget> CreateWindowRenderTarget(const WindowRenderTargetParameters& params) = 0;
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
        virtual core::Ptr<IWindowRenderTarget> CreateWindowRenderTarget(const WindowRenderTargetParameters& params) override;

        core::Ptr<Device> GetDevice() { return m_device; }
        core::Ptr<IWindowRenderTarget> GetWindowRenderTarget();

        core::Ptr<IDeviceBuffer> CreateDeviceBuffer(const DeviceBufferParams& params, edt::DenseArrayView<const uint8_t> data = edt::DenseArrayView<const uint8_t>());

	protected:

		struct RT {
			void Activate(Device* device);
            void Clear(Device* device, const float(&color)[4]);

			core::Ptr<Texture> rt;
			core::Ptr<Texture> ds;
		} m_renderTarget;

        
		core::Ptr<Device> m_device;

    private:
        bool m_fullscreen = false;
        core::Application* m_app = nullptr;
        std::vector<std::string> m_dependencies;
        core::Ptr<IWindowRenderTarget> m_windowRT;
	};
}
