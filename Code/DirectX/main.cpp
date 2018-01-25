#include <d3d11.h>
#include <stdexcept>

template<typename T>
class D3DPtr {
public:
	D3DPtr(T* p = nullptr) :
		m_p(p)
	{}

	D3DPtr(const D3DPtr&) = delete;

	D3DPtr(D3DPtr&& that) {
		MoveFrom<false>(std::move(that));
	}

	~D3DPtr() {
		Release<false>();
	}

	T* Get() {
		return m_p;
	}

	template<bool release_prev = true>
	void Set(T* ptr) {
		if constexpr (release_prev) {
			Release<false>();
		}
		m_p = ptr;
	}

	template<bool set_to_null>
	void Release() {
		if (m_p != nullptr) {
			m_p->Release();
			if constexpr (set_to_null) {
				m_p = nullptr;
			}
		}
	}

	template<bool release_prev = true>
	void MoveFrom(D3DPtr<T>&& that) {
		Set<release_prev>(that.m_p);
		that.m_p = nullptr;
	}

	T* operator->() const {
		return m_p;
	}

	D3DPtr& operator=(const D3DPtr&) = delete;

	D3DPtr& operator=(D3DPtr&& that) {
		MoveFrom<true>(std::move(that));
		return *this;
	}

private:
	T* m_p;
};

class Renderer {
public:
	void Initialize() {
		constexpr int width = 720;
		constexpr int height = 1080;
		constexpr int samples = 1;

		CreateDevice(width, height, samples);
		CreateDepthStencilTexture(width, height, samples);
		CreateRenderTarget(width, height, samples);
		CreateRenderTargetView(width, height, samples);
	}

private:
	void CreateDevice(int width, int height, int samples) {
		UINT createDeviceFlags = 0;
		D3D_DRIVER_TYPE driverTypes[] =
		{
			D3D_DRIVER_TYPE_HARDWARE,
			D3D_DRIVER_TYPE_REFERENCE,
		};
		const unsigned numDriverTypes = ARRAYSIZE(driverTypes);

		D3D_FEATURE_LEVEL featureLevels[] =
		{
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_10_0,
		};
		const unsigned numFeatureLevels = ARRAYSIZE(featureLevels);

		DXGI_SWAP_CHAIN_DESC sd {};
		sd.BufferCount = 2;
		sd.BufferDesc.Width = width;
		sd.BufferDesc.Height = height;
		sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.BufferDesc.RefreshRate.Numerator = 60;
		sd.BufferDesc.RefreshRate.Denominator = 1;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.SampleDesc.Count = samples;
		sd.SampleDesc.Quality = UINT(D3D11_STANDARD_MULTISAMPLE_PATTERN);
		sd.Windowed = false;
		sd.Flags = (!sd.Windowed) ? DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH : 0;

		HRESULT hr = S_OK;
		D3D_FEATURE_LEVEL selectedFeatureLevel;

		ID3D11Device* device = nullptr;
		ID3D11DeviceContext* deviceContext = nullptr;

		for (unsigned driverTypeIndex = 0;
			driverTypeIndex < numDriverTypes;
			driverTypeIndex++)
		{
			D3D_DRIVER_TYPE driverType = driverTypes[driverTypeIndex];
			hr = D3D11CreateDevice(
				nullptr,
				driverType,
				nullptr,
				createDeviceFlags,
				featureLevels,
				numFeatureLevels,
				D3D11_SDK_VERSION,
				&device,
				&selectedFeatureLevel,
				&deviceContext);

			if (SUCCEEDED(hr))
			{
				break;
			}
		}

		if (FAILED(hr))
		{
			throw std::runtime_error("Failed to create device");
		}

		m_device.Set(device);
		m_deviceContext.Set(deviceContext);
	}
	void CreateDepthStencilTexture(int width, int height, int samples) {
		m_depthStencil = CreateTexture(
			width, height, samples,
			DXGI_FORMAT_R32_TYPELESS,
			D3D11_USAGE_DEFAULT,
			D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE);
	}
	void CreateDepthStencilView(int samples) {
		ID3D11DepthStencilView* depthStencilView = nullptr;
		D3D11_DEPTH_STENCIL_VIEW_DESC descDSV {};
		descDSV.Format = DXGI_FORMAT_D32_FLOAT;
		descDSV.ViewDimension = (samples == 1) ? D3D11_DSV_DIMENSION_TEXTURE2D : D3D11_DSV_DIMENSION_TEXTURE2DMS;
		descDSV.Texture2D.MipSlice = 0;
		if (FAILED(m_device->CreateDepthStencilView(m_depthStencil.Get(), &descDSV, &depthStencilView)))
		{
			throw std::runtime_error("Failed to create depth stencil view");
		}
		m_depthStencilView.Set(depthStencilView);
	}
	void CreateRenderTarget(int width, int height, int samples) {
		m_renderTarget = CreateTexture(
			width, height, samples,
			DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM,
			D3D11_USAGE_DEFAULT,
			D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET);
	}
	void CreateRenderTargetView(int width, int height, int samples) {
		D3D11_RENDER_TARGET_VIEW_DESC desc;
		desc.Format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MipSlice = 0;
		ID3D11RenderTargetView* view;
		auto hResult = m_device->CreateRenderTargetView(m_renderTarget.Get(), &desc, &view);
		if (FAILED(hResult)) {
			throw std::runtime_error("Failed to create render target view");
		}
		m_renderTargetView.Set(view);
	}

	D3DPtr<ID3D11Texture2D> CreateTexture(int width, int height, int samples, DXGI_FORMAT format, D3D11_USAGE usage, UINT bindFlags, UINT cpuAccessFlags = 0) {
		ID3D11Texture2D* resultTexture = nullptr;
		D3D11_TEXTURE2D_DESC descDepth;
		descDepth.Width = width;
		descDepth.Height = height;
		descDepth.MipLevels = 1;
		descDepth.ArraySize = 1;
		descDepth.Format = format;
		descDepth.SampleDesc.Count = samples;
		descDepth.SampleDesc.Quality = UINT(D3D11_STANDARD_MULTISAMPLE_PATTERN);
		descDepth.Usage = usage;
		descDepth.BindFlags = bindFlags;
		descDepth.CPUAccessFlags = cpuAccessFlags;
		descDepth.MiscFlags = 0;

		auto depthStencilCreateCode = m_device->CreateTexture2D(&descDepth, nullptr, &resultTexture);
		if (FAILED(depthStencilCreateCode)) {
			throw std::runtime_error("Failed to create texture");
		}

		return D3DPtr<ID3D11Texture2D>(resultTexture);
	}

	D3DPtr<ID3D11Device> m_device;
	D3DPtr<ID3D11DeviceContext> m_deviceContext;
	D3DPtr<ID3D11Texture2D> m_depthStencil;
	D3DPtr<ID3D11DepthStencilView> m_depthStencilView;
	D3DPtr<ID3D11Texture2D> m_renderTarget;
	D3DPtr<ID3D11RenderTargetView> m_renderTargetView;
};

int main() {
	Renderer renderer;
	renderer.Initialize();

	Renderer r2;
	r2.Initialize();
	return 0;
}