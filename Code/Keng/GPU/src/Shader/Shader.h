#pragma once

#include "d3dcompiler.h"
#include "Keng/GPU/Shader/IShader.h"
#include "WinWrappers/ComPtr.h"
#include "WinWrappers/WinWrappers.h"
#include "EverydayTools/Exception/CallAndRethrow.h"
#include "EverydayTools/Exception/ThrowIfFailed.h"
#include "Device.h"
#include "CompileShader.h"

namespace keng::gpu::shader_details
{
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

    template<ShaderType shaderType>
    class ShaderBase : public core::RefCountImpl<IShaderT<shaderType>>
    {
    public:
        using Traits = shader_details::ShaderTraits<shaderType>;
        using Interface = typename Traits::Interface;

        ShaderBase(Device& device, ShaderVersion version, const ShaderParameters& parameters) {
            CallAndRethrowM + [&] {
                m_device = &device;
                Compile(parameters, version);
                Create();
            };
        }

        virtual ShaderType GetShaderType() const override {
            return shaderType;
        }

        virtual void AssignToPipeline() const override {
            Traits::Set(m_device->GetContext().Get(), m_shader.Get(), nullptr, 0);
        }

    protected:
        DevicePtr m_device;
        ComPtr<ID3D10Blob> m_bytecode;
        ComPtr<Interface> m_shader;

    private:
        void Compile(const ShaderParameters& parameters, ShaderVersion version) {
            CallAndRethrowM + [&] {
                m_bytecode = CompileShaderToBlob(shaderType, version, parameters);
            };
        }

        void Create() {
            CallAndRethrowM + [&] {
                m_shader = Traits::Create(m_device->GetDevice().Get(),
                    m_bytecode->GetBufferPointer(),
                    m_bytecode->GetBufferSize(),
                    nullptr);
            };
        }
    };
}

namespace keng::gpu
{
    template<ShaderType shaderType>
    class Shader : public shader_details::ShaderBase<shaderType>
    {
    public:
        using shader_details::ShaderBase<shaderType>::ShaderBase;
    };

    template<>
    class Shader<ShaderType::Vertex> : public shader_details::ShaderBase<ShaderType::Vertex>
    {
    public:
        using shader_details::ShaderBase<ShaderType::Vertex>::ShaderBase;
        virtual IInputLayoutPtr MakeDefaultInputLayout() override;
        void ReflectInputLayout(std::vector<D3D11_INPUT_ELEMENT_DESC>& layouts);
    };

    using VertexShader = Shader<ShaderType::Vertex>;
    using FragmentShader = Shader<ShaderType::Fragment>;
}
