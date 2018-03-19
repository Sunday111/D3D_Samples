#pragma once

#include "Keng/Systems/System.h"
#include "Keng/Systems/WindowSystem/MainWindow.h"
#include "EverydayTools/Observable.h"

#include "Keng/Rendering/Texture.h"
#include "Keng/Rendering/Effect.h"

#include "D3D_Tools/Device.h"
#include "D3D_Tools/SwapChain.h"
#include "D3D_Tools/Texture.h"
#include "D3D_Tools/Shader.h"
#include "D3D_Tools/BufferMapper.h"
#include "D3D_Tools/CrossDeviceBuffer.h"

namespace keng
{
	class Application;

	class IDevice : public IRefCountObject {
	public:
		virtual void* GetNativeDevice() const = 0;
		virtual void* GetNativeContext() const = 0;
		virtual ~IDevice() = default;
	};

	class ISwapChain : public IRefCountObject {
	public:
		virtual void* GetNativeInterface() const = 0;
		virtual ~ISwapChain() = default;
	};

	class Device :
		public RefCountImpl<IDevice>,
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
		public RefCountImpl<ISwapChain>,
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
		public System<GraphicsSystem>,
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

		virtual void Initialize(IApplication* app) override;

	protected:
		Application* m_app = nullptr;
        SystemParams ReadDefaultParams();

		struct ShaderInfo {
			void Activate(Device* device);

			ComPtr<ID3D11InputLayout> layout;
			IntrusivePtr<Effect> effect;
		};

		struct RT {
			void Activate(Device* device);

			std::unique_ptr<Texture> rt;
			IntrusivePtr<TextureView<ResourceViewType::RenderTarget>> rt_rtv;
			IntrusivePtr<TextureView<ResourceViewType::ShaderResource>> rt_srv;

			std::unique_ptr<Texture> ds;
			IntrusivePtr<TextureView<ResourceViewType::DepthStencil>> ds_dsv;
			IntrusivePtr<TextureView<ResourceViewType::ShaderResource>> ds_srv;
		} m_renderTarget;


		bool m_fullscreen = false;
		IntrusivePtr<Device> m_device;
		IntrusivePtr<SwapChain> m_swapchain;
		IntrusivePtr<TextureView<ResourceViewType::RenderTarget>> m_renderTargetView;
	};
}
