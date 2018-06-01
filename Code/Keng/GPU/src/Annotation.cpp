#include "Annotation.h"
#include "Device.h"

namespace keng::gpu
{
    Annotation::Annotation(Device& device) {
        m_device = &device;
        CallAndRethrowM + [&] {
            WinAPI<char>::ThrowIfError(device.GetContext()->QueryInterface(
                __uuidof(ID3DUserDefinedAnnotation),
                reinterpret_cast<void**>(m_annotation.Receive())));
        };
    }

    void Annotation::BeginEvent(const wchar_t* text) {
        m_annotation->BeginEvent(text);
    }

    void Annotation::EndEvent() {
        m_annotation->EndEvent();
    }
}
