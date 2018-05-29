#pragma once

#include "FwdDecl.h"
#include "EverydayTools/Array/ArrayView.h"
#include "Keng/GraphicsAPI/ShaderType.h"
#include "d3d11_1.h"
#include "d3dcompiler.h"
#include "Keng/GraphicsAPI/ShaderType.h"
#include "WinWrappers/ComPtr.h"

namespace keng::graphics_api
{
    enum class ShaderVersion
    {
        _5_0,
        _4_1,
        _4_0,
    };

    struct ShaderMacro
    {
        std::string_view name;
        std::string_view value;
    };

    namespace shader_details {
        template<typename Interface>
        using CreateShaderMethodType = HRESULT(ID3D11Device::*)(const void*, SIZE_T, ID3D11ClassLinkage*, Interface**);

        template<typename Interface>
        using SetShaderMethodType = void(ID3D11DeviceContext::*)(Interface*, ID3D11ClassInstance*const*, unsigned);

        template
        <
            typename InterfaceType,
            CreateShaderMethodType<InterfaceType> createMethod,
            SetShaderMethodType<InterfaceType> setMethod
        >
        struct ShaderTraitsBase
        {
            using Interface = InterfaceType;
            static ComPtr<Interface> Create(ID3D11Device* device, const void* bytecode, SIZE_T size, ID3D11ClassLinkage* linkage) {
                return CallAndRethrowM + [&] {
                    ComPtr<Interface> result;
                    WinAPI<char>::ThrowIfError((device->*createMethod)(bytecode, size, linkage, result.Receive()));
                    return result;
                };
            }

            static void Set(ID3D11DeviceContext* context, Interface* shader, ID3D11ClassInstance*const* instances, uint32_t count) {
                (context->*setMethod)(shader, instances, count);
            }
        };

        template<ShaderType shaderType>
        struct ShaderTraits;

        template<>
        struct ShaderTraits<ShaderType::Vertex> :
            public ShaderTraitsBase<
            ID3D11VertexShader,
            &ID3D11Device::CreateVertexShader,
            &ID3D11DeviceContext::VSSetShader>
        {
        };

        template<>
        struct ShaderTraits<ShaderType::Fragment> :
            public ShaderTraitsBase<
            ID3D11PixelShader,
            &ID3D11Device::CreatePixelShader,
            &ID3D11DeviceContext::PSSetShader>
        {
        };

        template<>
        struct ShaderTraits<ShaderType::Geometry> :
            public ShaderTraitsBase<
            ID3D11GeometryShader,
            &ID3D11Device::CreateGeometryShader,
            &ID3D11DeviceContext::GSSetShader>
        {
        };

        template<>
        struct ShaderTraits<ShaderType::Hull> :
            public ShaderTraitsBase<
            ID3D11HullShader,
            &ID3D11Device::CreateHullShader,
            &ID3D11DeviceContext::HSSetShader>
        {
        };

        template<>
        struct ShaderTraits<ShaderType::Domain> :
            public ShaderTraitsBase<
            ID3D11DomainShader,
            &ID3D11Device::CreateDomainShader,
            &ID3D11DeviceContext::DSSetShader>
        {
        };

        template<>
        struct ShaderTraits<ShaderType::Compute> :
            public ShaderTraitsBase<
            ID3D11ComputeShader,
            &ID3D11Device::CreateComputeShader,
            &ID3D11DeviceContext::CSSetShader>
        {
        };
    }

    const char* ShaderTypeToShaderTarget(ShaderType shaderType, ShaderVersion shaderVersion);
    ComPtr<ID3DBlob> CompileShaderToBlob(const char* code, const char* entryPoint, ShaderType shaderType, ShaderVersion shaderVersion, edt::SparseArrayView<const ShaderMacro> definitionsView = edt::SparseArrayView<const ShaderMacro>());

    template
    <
        ShaderType shaderType,
        template<ShaderType> typename Derived,
        typename Enable = void
    >
    class ShaderReflectionMixin
    {
    };

    template
    <
        ShaderType shaderType,
        template<ShaderType> typename Derived
    >
    class ShaderReflectionMixin<shaderType, Derived, std::enable_if_t<shaderType == ShaderType::Vertex>>
    {
    public:
        void ReflectInputLayout(std::vector<D3D11_INPUT_ELEMENT_DESC>& layouts) {
            CallAndRethrowM + [&] {
                auto& this_ = static_cast<Derived<shaderType>&>(*this);
                auto& bytecode = this_.bytecode;

                edt::ThrowIfFailed<std::logic_error>(bytecode != nullptr, "Bytecode is nullptr!");
                ComPtr<ID3D11ShaderReflection> pShaderReflector;
                WinAPI<char>::ThrowIfError(
                    D3DReflect(bytecode->GetBufferPointer(), bytecode->GetBufferSize(),
                        __uuidof(ID3D11ShaderReflection), (void**)pShaderReflector.Receive()));

                D3D11_SHADER_DESC shaderDesc;
                pShaderReflector->GetDesc(&shaderDesc);

                for (uint32_t i = 0; i < shaderDesc.InputParameters; ++i) {
                    D3D11_SIGNATURE_PARAMETER_DESC paramDesc;
                    pShaderReflector->GetInputParameterDesc(i, &paramDesc);

                    // fill out input element desc
                    D3D11_INPUT_ELEMENT_DESC elementDesc;
                    elementDesc.SemanticName = paramDesc.SemanticName;
                    elementDesc.SemanticIndex = paramDesc.SemanticIndex;
                    elementDesc.InputSlot = 0;
                    elementDesc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
                    elementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
                    elementDesc.InstanceDataStepRate = 0;

                    // determine DXGI format
                    if (paramDesc.Mask == 1) {
                        switch (paramDesc.ComponentType) {
                        case D3D_REGISTER_COMPONENT_UINT32:  elementDesc.Format = DXGI_FORMAT_R32_UINT;  break;
                        case D3D_REGISTER_COMPONENT_SINT32:  elementDesc.Format = DXGI_FORMAT_R32_SINT;  break;
                        case D3D_REGISTER_COMPONENT_FLOAT32: elementDesc.Format = DXGI_FORMAT_R32_FLOAT; break;
                        }
                    } else if (paramDesc.Mask <= 3) {
                        switch (paramDesc.ComponentType) {
                        case D3D_REGISTER_COMPONENT_UINT32:  elementDesc.Format = DXGI_FORMAT_R32G32_UINT;  break;
                        case D3D_REGISTER_COMPONENT_SINT32:  elementDesc.Format = DXGI_FORMAT_R32G32_SINT;  break;
                        case D3D_REGISTER_COMPONENT_FLOAT32: elementDesc.Format = DXGI_FORMAT_R32G32_FLOAT; break;
                        }
                    } else if (paramDesc.Mask <= 7) {
                        switch (paramDesc.ComponentType) {
                        case D3D_REGISTER_COMPONENT_UINT32:  elementDesc.Format = DXGI_FORMAT_R32G32B32_UINT;  break;
                        case D3D_REGISTER_COMPONENT_SINT32:  elementDesc.Format = DXGI_FORMAT_R32G32B32_SINT;  break;
                        case D3D_REGISTER_COMPONENT_FLOAT32: elementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT; break;
                        }
                    } else if (paramDesc.Mask <= 15) {
                        switch (paramDesc.ComponentType) {
                        case D3D_REGISTER_COMPONENT_UINT32:  elementDesc.Format = DXGI_FORMAT_R32G32B32A32_UINT;  break;
                        case D3D_REGISTER_COMPONENT_SINT32:  elementDesc.Format = DXGI_FORMAT_R32G32B32A32_SINT;  break;
                        case D3D_REGISTER_COMPONENT_FLOAT32: elementDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT; break;
                        }
                    }

                    layouts.push_back(elementDesc);
                }
            };
        }
    };

    template<ShaderType shaderType>
    class Shader :
        public core::RefCountImpl<core::IRefCountObject>,
        public ShaderReflectionMixin<shaderType, ::keng::graphics_api::Shader>
    {
    public:
        using Traits = shader_details::ShaderTraits<shaderType>;
        using Interface = typename Traits::Interface;

        void Compile(const char* code, const char* entryPoint, ShaderVersion shaderVersion, edt::SparseArrayView<const ShaderMacro> definitions =
            edt::SparseArrayView<const ShaderMacro>()) {
            CallAndRethrowM + [&] {
                bytecode = CompileShaderToBlob(code, entryPoint, shaderType, shaderVersion, definitions);
            };
        }

        void Create(ID3D11Device* device) {
            CallAndRethrowM + [&] {
                shader = Traits::Create(device,
                    bytecode->GetBufferPointer(),
                    bytecode->GetBufferSize(),
                    nullptr);
            };
        }

        ComPtr<ID3D10Blob> bytecode;
        ComPtr<Interface> shader;
    };
}
