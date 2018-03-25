#pragma once

#include "Keng/Core/ISystem.h"
#include "Keng/WindowSystem/IWindowListener.h"

#include "Keng/Graphics/Device.h"
#include "Keng/Graphics/SwapChain.h"
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

namespace keng::graphics
{
    class IDeviceBuffer;
    class DeviceBufferParams;

	class GraphicsSystem :
        public core::ISystem,
        public window_system::IWindowListener,
		public Observable<GraphicsSystem, IGraphicsListener>
	{
	public:
		GraphicsSystem();

		static const char* GetGUID();

		void OnWindowResize(int w, int h) override;

        // ISystem
        virtual bool ForEachSystemDependency(bool(*pfn)(const char* systemGUID, void* context), void* context) override;
        virtual const char* GetSystemGUID() override;
		virtual void Initialize(core::IApplication* app) override;

        std::shared_ptr<Device> GetDevice() { return m_device; }

        core::Ptr<IDeviceBuffer> CreateDeviceBuffer(const DeviceBufferParams& params, edt::DenseArrayView<const uint8_t> data = edt::DenseArrayView<const uint8_t>());

	protected:

		struct RT {
			void Activate(Device* device);
            void Clear(Device* device, const float(&color)[4]);

			core::Ptr<Texture> rt;
			core::Ptr<Texture> ds;
		} m_renderTarget;

        
		std::shared_ptr<Device> m_device;
		std::shared_ptr<SwapChain> m_swapchain;
		core::Ptr<TextureView<ResourceViewType::RenderTarget>> m_renderTargetView;

    private:
        bool m_fullscreen = false;
        core::Application* m_app = nullptr;
        std::vector<std::string> m_dependencies;
	};
}
