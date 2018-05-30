#include "InputLayout.h"
#include "EverydayTools/Exception/ThrowIfFailed.h"
#include "EverydayTools/Exception/CallAndRethrow.h"

namespace keng::gpu
{
    InputLayout::InputLayout(Device& device, ComPtr<ID3D11InputLayout> layout) :
        m_layout(layout)
    {
        CallAndRethrowM + [&] {
            edt::ThrowIfFailed(layout != nullptr);
            m_device = &device;
        };
    }

    void InputLayout::AssignToPipeline() {
        m_device->GetContext()->IASetInputLayout(m_layout.Get());
    }

    ComPtr<ID3D11InputLayout> InputLayout::GetLayout() const {
        return m_layout;
    }
}
