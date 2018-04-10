#include "Annotation.h"
#include "Device.h"

namespace keng::graphics
{
    Annotation::Annotation(Device& device) {
        m_device = &device;
        m_annotation = m_device->CreateAnnotation();
    }

    void Annotation::BeginEvent(std::basic_string_view<wchar_t> text) {
        m_annotation->BeginEvent(text.data());
    }

    void Annotation::EndEvent() {
        m_annotation->EndEvent();
    }
}
