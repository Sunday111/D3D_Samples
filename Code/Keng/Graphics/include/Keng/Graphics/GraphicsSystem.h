#pragma once

#include "Keng/Core/Systems/ISystem.h"
#include "Keng/Core/Systems/WindowSystem/MainWindow.h"
#include "EverydayTools/Observable.h"

#include "Keng/Graphics/Texture.h"
#include "Keng/Graphics/Effect.h"

#include "D3D_Tools/Device.h"
#include "D3D_Tools/SwapChain.h"
#include "D3D_Tools/Texture.h"
#include "D3D_Tools/Shader.h"
#include "D3D_Tools/BufferMapper.h"
#include "D3D_Tools/CrossDeviceBuffer.h"

namespace keng
{
	class Application;

	class IDevice
    {
	public:
		virtual void* GetNativeDevice() const = 0;
		virtual void* GetNativeContext() const = 0;
		virtual ~IDevice() = default;
	};

	class ISwapChain
    {
	public:
		virtual void* GetNativeInterface() const = 0;
		virtual ~ISwapChain() = default;
	};

	class Device :
		public IDevice,
		public d3d_tools::Device
	{
	public:
		using d3d_tools::Device::Device;

		void* GetNativeDevice() const override {
			return GetDevice();
		}

		void* GetNativeContext() const override {
			return GetContext();
		}
	};

	class SwapChain :
		public ISwapChain,
		public d3d_tools::SwapChain
	{
	public:
		using d3d_tools::SwapChain::SwapChain;

		void* GetNativeInterface() const {
			return GetInterface();
		}
	};

	class IGraphicsListener {
	public:
		virtual void BeforeRendering() = 0;
		virtual void AfterRendering() = 0;
		virtual ~IGraphicsListener() = default;
	};

	class GraphicsSystem :
		public ISystem,
		public IMainWindowListener,
		public Observable<GraphicsSystem, IGraphicsListener>
	{
	public:
		GraphicsSystem();

		struct SystemParams {
			bool debugDevice = false;
			bool noDeviceMultithreading = false;
		};

		static const char* GetGUID();

		void OnWindowResize(int w, int h) override;

        // ISystem
        virtual bool ForEachSystemDependency(bool(*pfn)(const char* systemGUID, void* context), void* context) override;
        virtual const char* GetSystemGUID() override;
		virtual void Initialize(IApplication* app) override;

	protected:
        std::vector<std::string> m_dependencies;
		Application* m_app = nullptr;
        SystemParams ReadDefaultParams();

		struct ShaderInfo {
			void Activate(Device* device);

			ComPtr<ID3D11InputLayout> layout;
			std::shared_ptr<Effect> effect;
		};

		struct RT {
			void Activate(Device* device);

			std::unique_ptr<Texture> rt;
			std::shared_ptr<TextureView<ResourceViewType::RenderTarget>> rt_rtv;
			std::shared_ptr<TextureView<ResourceViewType::ShaderResource>> rt_srv;

			std::unique_ptr<Texture> ds;
			std::shared_ptr<TextureView<ResourceViewType::DepthStencil>> ds_dsv;
			std::shared_ptr<TextureView<ResourceViewType::ShaderResource>> ds_srv;
		} m_renderTarget;


		bool m_fullscreen = false;
		std::shared_ptr<Device> m_device;
		std::shared_ptr<SwapChain> m_swapchain;
		std::shared_ptr<TextureView<ResourceViewType::RenderTarget>> m_renderTargetView;
	};
}
