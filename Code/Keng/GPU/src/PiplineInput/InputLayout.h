#pragma once

#include "Keng/GPU/PipelineInput/IInputLayout.h"
#include "d3d11_1.h"
#include "WinWrappers/ComPtr.h"
#include "Device.h"

namespace keng::graphics::gpu
{
    class InputLayout : public core::RefCountImpl<IInputLayout>
    {
    public:
        InputLayout(Device& device, ComPtr<ID3D11InputLayout> layout);

        // InputLayout
        virtual void AssignToPipeline() override;
        // InputLayout

        ComPtr<ID3D11InputLayout> GetLayout() const;

    private:
        ComPtr<ID3D11InputLayout> m_layout;
        DevicePtr m_device;
    };
}