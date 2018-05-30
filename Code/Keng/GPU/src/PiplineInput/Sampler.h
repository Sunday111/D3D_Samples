#pragma once

#include "d3d11.h"
#include "WinWrappers/ComPtr.h"
#include "FwdDecl.h"
#include "Keng/GPU/PipelineInput/ISampler.h"

namespace keng::gpu
{
    class Sampler : public core::RefCountImpl<ISampler>
    {
    public:
        Sampler(Device& device, const SamplerParameters& params);
        virtual void AssignToPipeline(ShaderType shaderType, size_t index) override;

    private:
        DevicePtr m_device;
        ComPtr<ID3D11SamplerState> m_sampler;
    };
}
