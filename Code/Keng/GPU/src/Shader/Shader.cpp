#include "Shader.h"
#include "PiplineInput/InputLayout.h"
#include "Device.h"
#include "EverydayTools/Array/ArrayViewVector.h"

namespace keng::graphics::gpu
{
    IInputLayoutPtr VertexShader::MakeDefaultInputLayout() {
        return CallAndRethrowM + [&] {
            std::vector<D3D11_INPUT_ELEMENT_DESC> layouts;
            ReflectInputLayout(layouts);
            auto layout = m_device->CreateInputLayout(edt::MakeArrayView(layouts), m_bytecode.Get());
            return InputLayoutPtr::MakeInstance(*m_device, layout);
        };
    }

    void VertexShader::ReflectInputLayout(std::vector<D3D11_INPUT_ELEMENT_DESC>& layouts) {
        CallAndRethrowM + [&] {
            edt::ThrowIfFailed<std::logic_error>(m_bytecode != nullptr, "Bytecode is nullptr!");
            ComPtr<ID3D11ShaderReflection> pShaderReflector;
            WinAPI<char>::ThrowIfError(
                D3DReflect(m_bytecode->GetBufferPointer(), m_bytecode->GetBufferSize(),
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
}
