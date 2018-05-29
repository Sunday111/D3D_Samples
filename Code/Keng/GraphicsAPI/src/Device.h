#pragma once

#include "Keng/GraphicsAPI/FwdDecl.h"
#include "Keng/Core/IRefCountObject.h"
#include "Keng/GraphicsAPI/IDevice.h"
#include "WinWrappers/ComPtr.h"
#include "WinWrappers/WinWrappers.h"
#include "d3d11_1.h"
#include "Resource/Shader/Shader.h"
#include "Resource/Texture/TextureView.h"
#include "EverydayTools/Array/ArrayView.h"

namespace keng::graphics_api
{
    class Device :
        public core::RefCountImpl<IDevice>
    {
    public:
        Device(const DeviceParameters& parameters);

        void* GetNativeDevice() const override;
        void* GetNativeContext() const override;

        const ComPtr<ID3D11Device>& GetDevice() const;
        const ComPtr<ID3D11DeviceContext>& GetContext() const;

        template<ShaderType shaderType>
        void SetShader(Shader<shaderType>& shader) {
            CallAndRethrowM + [&] {
                using Traits = shader_details::ShaderTraits<shaderType>;
                Traits::Set(
                    m_deviceContext.Get(),
                    shader.shader.Get(),
                    nullptr,
                    0);
            };
        }

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