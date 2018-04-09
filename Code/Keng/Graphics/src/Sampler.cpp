#include "Sampler.h"
#include "EverydayTools/Exception/CallAndRethrow.h"
#include "Device.h"
#include "WinWrappers/WinWrappers.h"
#include "D3D_11/EnumConverter.h"
#include "Keng/Graphics/SamplerParameters.h"

namespace keng::graphics
{
    Sampler::Sampler(Device& device, const SamplerParameters& params) {
        CallAndRethrowM + [&] {
            m_device = &device;
            CD3D11_SAMPLER_DESC desc(D3D11_DEFAULT);
            desc.AddressU = d3d::ConvertAddressMode(params.addressU);
            desc.AddressV = d3d::ConvertAddressMode(params.addressV);
            desc.AddressW = d3d::ConvertAddressMode(params.addressW);
            std::copy(params.borderColor, params.borderColor + 4, desc.BorderColor);
            desc.Filter = d3d::ConvertFilteringMode(params.filter);

            if (params.filter == FilteringMode::Anisotropic) {
                desc.MaxAnisotropy = 16;
            }
            
            WinAPI<char>::ThrowIfError(m_device->GetDevice()->CreateSamplerState(&desc, m_sampler.Receive()));
        };
    }

    void Sampler::AssignToPipeline(ShaderType shaderType, size_t index) {
        CallAndRethrowM + [&] {
            m_device->SetSampler(index, m_sampler.Get(), (d3d_tools::ShaderType)shaderType);
        };
    }
}
