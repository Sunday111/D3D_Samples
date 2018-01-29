#pragma once

#include <d3d11.h>
#include "ComPtr.h"
#include "ResourceSystem.h"

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


template<typename Interface_>
struct ShaderTraisBase {
    using Interface = Interface_;
    using CreateMethodType = HRESULT(ID3D11Device::*)(const void*, SIZE_T, ID3D11ClassLinkage*, Interface**);
    using SetMethodType = void(ID3D11DeviceContext::*)(Interface*, ID3D11ClassInstance*const*, unsigned);
};

template<ShaderType shaderType>
struct ShaderTraits;

template<>
struct ShaderTraits<ShaderType::Vertex> :
    public ShaderTraisBase<ID3D11VertexShader>
{
    static CreateMethodType CreateMethod() { return &ID3D11Device::CreateVertexShader; }
    static SetMethodType SetMethod() { return &ID3D11DeviceContext::VSSetShader; }
};

template<>
struct ShaderTraits<ShaderType::Pixel> :
    public ShaderTraisBase<ID3D11PixelShader>
{
    static CreateMethodType CreateMethod() { return &ID3D11Device::CreatePixelShader; }
    static SetMethodType SetMethod() { return &ID3D11DeviceContext::PSSetShader; }
};

template<>
struct ShaderTraits<ShaderType::Geometry> :
    public ShaderTraisBase<ID3D11GeometryShader>
{
    static CreateMethodType CreateMethod() { return &ID3D11Device::CreateGeometryShader; }
    static SetMethodType SetMethod() { return &ID3D11DeviceContext::GSSetShader; }
};

template<>
struct ShaderTraits<ShaderType::Hull> :
    public ShaderTraisBase<ID3D11HullShader>
{
    static CreateMethodType CreateMethod() { return &ID3D11Device::CreateHullShader; }
    static SetMethodType SetMethod() { return &ID3D11DeviceContext::HSSetShader; }
};

template<>
struct ShaderTraits<ShaderType::Domain> :
    public ShaderTraisBase<ID3D11DomainShader>
{
    static CreateMethodType CreateMethod() { return &ID3D11Device::CreateDomainShader; }
    static SetMethodType SetMethod() { return &ID3D11DeviceContext::DSSetShader; }
};

template<>
struct ShaderTraits<ShaderType::Compute> :
    public ShaderTraisBase<ID3D11ComputeShader>
{
    static CreateMethodType CreateMethod() { return &ID3D11Device::CreateComputeShader; }
    static SetMethodType SetMethod() { return &ID3D11DeviceContext::CSSetShader; }
};

template<ShaderType shaderType>
class Shader
{
public:
    using Traits = ShaderTraits<shaderType>;
    using Interface = typename Traits::Interface;
    ComPtr<ID3D10Blob> bytecode;
    ComPtr<Interface> shader;
};