#include "Sampler.h"
#include "EverydayTools/Exception/CallAndRethrow.h"
#include "EverydayTools/Exception/CheckedCast.h"
#include "Device.h"
#include "WinWrappers/WinWrappers.h"
#include "EnumConverter.h"
#include "Keng/GraphicsCommon/SamplerParameters.h"

namespace keng::graphics::gpu
{
    Sampler::Sampler(Device& device, const SamplerParameters& params) {
        CallAndRethrowM + [&] {
            m_device = &device;
            CD3D11_SAMPLER_DESC desc(D3D11_DEFAULT);
            desc.AddressU = ConvertAddressMode(params.addressU);
            desc.AddressV = ConvertAddressMode(params.addressV);
            desc.AddressW = ConvertAddressMode(params.addressW);
            std::copy(params.borderColor, params.borderColor + 4, desc.BorderColor);
            desc.Filter = ConvertFilteringMode(params.filter);

            if (params.filter == FilteringMode::Anisotropic) {
                desc.MaxAnisotropy = 16;
            }
            
            WinAPI<char>::ThrowIfError(m_device->GetDevice()->CreateSamplerState(&desc, m_sampler.Receive()));
        };
    }

    void Sampler::AssignToPipeline(const ShaderType& shaderType, size_t index) {
        CallAndRethrowM + [&] {
            m_device->SetSampler(edt::CheckedCast<uint32_t>(index), m_sampler.Get(), shaderType);
        };
    }
}
