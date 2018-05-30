#pragma once

#include "Keng/GraphicsAPI/IInputLayout.h"
#include "d3d11_1.h"
#include "WinWrappers/ComPtr.h"
#include "Device.h"

namespace keng::graphics_api
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