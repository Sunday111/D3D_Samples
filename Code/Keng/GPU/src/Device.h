#pragma once

#include "Keng/GPU/FwdDecl.h"
#include "Keng/GPU/Shader/ShaderType.h"
#include "Keng/GPU/IDevice.h"
#include "Keng/Core/IRefCountObject.h"
#include "WinWrappers/ComPtr.h"
#include "WinWrappers/WinWrappers.h"
#include "d3d11_1.h"
#include "Resource/Texture/TextureView.h"
#include "EverydayTools/Array/ArrayView.h"

namespace keng::gpu
{
    class Device :
        public core::RefCountImpl<IDevice>
    {
    public:
        Device(const DeviceParameters& parameters);

        // IDevice
        virtual IWindowRenderTargetPtr CreateWindowRenderTarget(const WindowRenderTargetParameters& params) override;
        virtual ITextureRenderTargetPtr CreateTextureRenderTarget(const TextureRenderTargetParameters& params) override;
        virtual IDepthStencilPtr CreateDepthStencil(const DepthStencilParameters& params) override;
        virtual ITexturePtr CreateTexture(const TextureParameters& params) override;
        virtual IShaderPtr CreateShader(ShaderType type, const ShaderParameters& parameters) override;
        virtual IDeviceBufferPtr CreateDeviceBuffer(const DeviceBufferParameters& params, edt::DenseArrayView<const uint8_t> data) override;
        virtual void SetTopology(PrimitiveTopology topo) override;
        void* GetNativeDevice() const override;
        void* GetNativeContext() const override;
        // ~IDevice

        const ComPtr<ID3D11Device>& GetDevice() const;
        const ComPtr<ID3D11DeviceContext>& GetContext() const;

        void SetRenderTarget(TextureView<ResourceViewType::RenderTarget>& rtv, TextureView<ResourceViewType::DepthStencil>* dsv = nullptr);
        void SetViewports(edt::DenseArrayView<const D3D11_VIEWPORT> viewports);
        void Draw(unsigned vertexCount, unsigned startvert = 0);
        void SetInputLayout(ID3D11InputLayout* layout);
        void SetShaderResource(uint32_t slot, ShaderType shaderType, ID3D11ShaderResourceView* view);
        void SetSampler(uint32_t slot, ID3D11SamplerState* sampler, ShaderType shaderType);
        void SetVertexBuffer(ID3D11Buffer* buffer, unsigned stride, unsigned offset);
        void SetConstantBuffer(ID3D11Buffer* buffer, ShaderType shaderType);
        void SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY topo);
        ComPtr<ID3DUserDefinedAnnotation> Device::CreateAnnotation() const;
        ComPtr<ID3D11InputLayout> Device::CreateInputLayout(edt::DenseArrayView<const D3D11_INPUT_ELEMENT_DESC> descriptors, ID3D10Blob* shader);
        ComPtr<ID3D11InputLayout> Device::CreateInputLayout(const D3D11_INPUT_ELEMENT_DESC* elementDescriptor, uint32_t elementsCount, ID3D10Blob* shader);
        ComPtr<ID3D11Buffer> Device::CreateBuffer(D3D11_BUFFER_DESC desc, const void* initialData = nullptr);

    private:
        ComPtr<ID3D11Device> m_device;
        ComPtr<ID3D11DeviceContext> m_deviceContext;
    };
}