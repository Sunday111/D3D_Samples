#include "Device.h"
#include "Keng/Graphics/DeviceParameters.h"
#include "EverydayTools/Exception/ThrowIfFailed.h"
#include "EverydayTools/Exception/CallAndRethrow.h"
#include "d3d11shader.h"
#include "d3dcompiler.h"

namespace keng::graphics
{
    Device::Device(const DeviceParameters& params) {
        CallAndRethrowM + [&] {
            unsigned flags = 0;
            if (params.debugDevice) {
                flags |= D3D11_CREATE_DEVICE_DEBUG;
            }
            if (params.noDeviceMultithreading) {
                flags |= D3D11_CREATE_DEVICE_SINGLETHREADED;
                flags |= D3D11_CREATE_DEVICE_PREVENT_INTERNAL_THREADING_OPTIMIZATIONS;
            }
            WinAPI<char>::ThrowIfError(D3D11CreateDevice(
                nullptr,
                D3D_DRIVER_TYPE_HARDWARE,
                nullptr,
                flags,
                nullptr,
                0,
                D3D11_SDK_VERSION,
                m_device.Receive(),
                nullptr,
                m_deviceContext.Receive()));
        };
    }

    void* Device::GetNativeDevice() const {
        return GetDevice().Get();
    }

    void* Device::GetNativeContext() const {
        return GetContext().Get();
    }

    const ComPtr<ID3D11Device>& Device::GetDevice() const {
        return m_device;
    }

    const ComPtr<ID3D11DeviceContext>& Device::GetContext() const {
        return m_deviceContext;
     }

     void Device::SetRenderTarget(TextureView<ResourceViewType::RenderTarget>& rtv, TextureView<ResourceViewType::DepthStencil>* dsv) {
          auto pRTV = rtv.GetView();
          ID3D11DepthStencilView* pDSV = nullptr;
          if (dsv) {
               pDSV = dsv->GetView();
          }
          m_deviceContext->OMSetRenderTargets(1, &pRTV, pDSV);
     }

     void Device::SetViewports(edt::DenseArrayView<const D3D11_VIEWPORT> viewports) {
          m_deviceContext->RSSetViewports(
               static_cast<UINT>(viewports.GetSize()),
               viewports.GetData());
     }

     void Device::Draw(unsigned vertexCount, unsigned startvert) {
          m_deviceContext->Draw(vertexCount, startvert);
     }

     void Device::SetInputLayout(ID3D11InputLayout* layout) {
          m_deviceContext->IASetInputLayout(layout);
     }

     void Device::SetShaderResource(uint32_t slot, ShaderType shaderType, ID3D11ShaderResourceView* view) {
          using Method = void (ID3D11DeviceContext::*)(UINT, UINT, ID3D11ShaderResourceView* const *);
          Method method = nullptr;

          switch (shaderType)
          {
          case ShaderType::Compute:
               method = &ID3D11DeviceContext::CSSetShaderResources;
               break;
          case ShaderType::Domain:
               method = &ID3D11DeviceContext::DSSetShaderResources;
               break;
          case ShaderType::Geometry:
               method = &ID3D11DeviceContext::GSSetShaderResources;
               break;
          case ShaderType::Hull:
               method = &ID3D11DeviceContext::HSSetShaderResources;
               break;
          case ShaderType::Fragment:
               method = &ID3D11DeviceContext::PSSetShaderResources;
               break;
          case ShaderType::Vertex:
               method = &ID3D11DeviceContext::VSSetShaderResources;
               break;
          }

          edt::ThrowIfFailed(method != nullptr, "Not implemented for this shader type");
          (*m_deviceContext.*method)(slot, 1, &view);
     }

     void Device::SetSampler(uint32_t slot, ID3D11SamplerState* sampler, ShaderType shaderType) {
          using Method = void (ID3D11DeviceContext::*)(UINT, UINT, ID3D11SamplerState* const *);
          Method method = nullptr;

          switch (shaderType)
          {
          case ShaderType::Compute:
               method = &ID3D11DeviceContext::CSSetSamplers;
               break;
          case ShaderType::Domain:
               method = &ID3D11DeviceContext::DSSetSamplers;
               break;
          case ShaderType::Geometry:
               method = &ID3D11DeviceContext::GSSetSamplers;
               break;
          case ShaderType::Hull:
               method = &ID3D11DeviceContext::HSSetSamplers;
               break;
          case ShaderType::Fragment:
               method = &ID3D11DeviceContext::PSSetSamplers;
               break;
          case ShaderType::Vertex:
               method = &ID3D11DeviceContext::VSSetSamplers;
               break;
          }

          edt::ThrowIfFailed(method != nullptr, "Not implemented for this shader type");
          (*m_deviceContext.*method)(slot, 1, &sampler);
     }

     void Device::SetVertexBuffer(ID3D11Buffer* buffer, unsigned stride, unsigned offset) {
          m_deviceContext->IASetVertexBuffers(0, 1, &buffer, &stride, &offset);
     }

     void Device::SetConstantBuffer(ID3D11Buffer* buffer, ShaderType shaderType) {
          using Method = void (ID3D11DeviceContext::*)(UINT, UINT, ID3D11Buffer* const *);
          Method method = nullptr;

          switch (shaderType)
          {
          case ShaderType::Compute:
               method = &ID3D11DeviceContext::CSSetConstantBuffers;
               break;
          case ShaderType::Domain:
               method = &ID3D11DeviceContext::DSSetConstantBuffers;
               break;
          case ShaderType::Geometry:
               method = &ID3D11DeviceContext::GSSetConstantBuffers;
               break;
          case ShaderType::Hull:
               method = &ID3D11DeviceContext::HSSetConstantBuffers;
               break;
          case ShaderType::Fragment:
               method = &ID3D11DeviceContext::PSSetConstantBuffers;
               break;
          case ShaderType::Vertex:
               method = &ID3D11DeviceContext::VSSetConstantBuffers;
               break;
          }
          edt::ThrowIfFailed(method != nullptr, "Not implemented for this shader type");
          (*m_deviceContext.*method)(0, 1, &buffer);
     }

     void Device::SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY topo) {
          m_deviceContext->IASetPrimitiveTopology(topo);
     }

     ComPtr<ID3DUserDefinedAnnotation> Device::CreateAnnotation() const {
          return CallAndRethrowM + [&] {
               ComPtr<ID3DUserDefinedAnnotation> annotation;
               WinAPI<char>::ThrowIfError(m_deviceContext->QueryInterface(
                    __uuidof(ID3DUserDefinedAnnotation),
                    reinterpret_cast<void**>(annotation.Receive())));
               return annotation;
          };
     }

     ComPtr<ID3D11InputLayout> Device::CreateInputLayout(edt::DenseArrayView<const D3D11_INPUT_ELEMENT_DESC> descriptors, ID3D10Blob* shader) {
          return CreateInputLayout(descriptors.GetData(), static_cast<uint32_t>(descriptors.GetSize()), shader);
     }

     ComPtr<ID3D11InputLayout> Device::CreateInputLayout(const D3D11_INPUT_ELEMENT_DESC* elementDescriptor, uint32_t elementsCount, ID3D10Blob* shader) {
          return CallAndRethrowM + [&] {
               ComPtr<ID3D11InputLayout> result;
               auto onCreationError = [&](std::string&& briefErrorMessage) {
                    // Try to get adddition information about shader input layout
                    // to simplify debugging those annoying unclear errors

                    std::string reflectionExtraInfo;
                    auto onReflectError = [&reflectionExtraInfo](std::string&& briefErrorMessage) {
                         reflectionExtraInfo = "Failed to reflect shader: " + briefErrorMessage;
                    };

                    ComPtr<ID3D11ShaderReflection> pShaderReflector;
                    bool error = WinAPI<char>::HandleError(
                         D3DReflect(shader->GetBufferPointer(), shader->GetBufferSize(),
                              __uuidof(ID3D11ShaderReflection), (void**)pShaderReflector.Receive()),
                         onReflectError);

                    if (!error) {
                         // TODO:
                    }

                    throw std::runtime_error(
                         std::string("Failed to create input layout: ") +
                         briefErrorMessage + '\n' +
                         "Shader reflection information: \n" +
                         reflectionExtraInfo
                    );
               };
               WinAPI<char>::HandleError(
                    m_device->CreateInputLayout(
                         elementDescriptor, elementsCount,
                         shader->GetBufferPointer(), shader->GetBufferSize(),
                         result.Receive()),
                    onCreationError);
               return result;
          };
     }

     ComPtr<ID3D11Buffer> Device::CreateBuffer(D3D11_BUFFER_DESC desc, const void* initialData) {
          ComPtr<ID3D11Buffer> buffer;
          D3D11_SUBRESOURCE_DATA subresourceData;
          const D3D11_SUBRESOURCE_DATA* pSubresourceData = nullptr;
          if (initialData) {
               subresourceData.pSysMem = initialData;
               subresourceData.SysMemPitch = 0;
               subresourceData.SysMemSlicePitch = 0;
               pSubresourceData = &subresourceData;
          }
          WinAPI<char>::ThrowIfError(m_device->CreateBuffer(&desc, pSubresourceData, buffer.Receive()));
          return buffer;
     }
}

