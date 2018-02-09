#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include "ComPtr.h"
#include "ResourceSystem.h"
#include "EverydayTools\Exception\CallAndRethrow.h"
#include "WinWrappers\WinWrappers.h"
#include "RefCounter.h"

class Device;

enum class ShaderType {
    Compute,
    Domain,
    Geometry,
    Hull,
    Pixel,
    Vertex
};

enum class ShaderVersion {
    _5_0,
    _4_1,
    _4_0,
};

class IShader : public IRefCountObject
{
public:
    virtual ~IShader() = default;
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
    struct ShaderTraits<ShaderType::Pixel> :
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

ComPtr<ID3DBlob> CompileShaderToBlob(const char* code, const char* entryPoint, ShaderType shaderType, ShaderVersion shaderVersion);

template<ShaderType shaderType>
class Shader : public RefCountImpl<IShader>
{
public:
    using Traits = shader_details::ShaderTraits<shaderType>;
    using Interface = typename Traits::Interface;

    void Compile(const char* code, const char* entryPoint, ShaderVersion shaderVersion) {
        CallAndRethrowM + [&] {
            bytecode = CompileShaderToBlob(code, entryPoint, shaderType, shaderVersion);
        };
    }

    void Create(ID3D11Device* device) {
        shader = Traits::Create(device,
            bytecode->GetBufferPointer(),
            bytecode->GetBufferSize(),
            nullptr);
    }

    ComPtr<ID3D10Blob> bytecode;
    ComPtr<Interface> shader;
};
