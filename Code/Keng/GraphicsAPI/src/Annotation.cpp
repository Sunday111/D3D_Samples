#include "Annotation.h"
#include "Device.h"

namespace keng::graphics_api
{
    Annotation::Annotation(Device& device) {
        m_device = &device;
        m_annotation = m_device->CreateAnnotation();
    }

    void Annotation::BeginEvent(const wchar_t* text) {
        m_annotation->BeginEvent(text);
    }

    void Annotation::EndEvent() {
        m_annotation->EndEvent();
    }
}
