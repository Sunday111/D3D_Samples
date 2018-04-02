#include "Annotation.h"
#include "Keng/Graphics/Device.h"

namespace keng::graphics
{
    Annotation::Annotation(Device& device) {
        m_device = &device;
        m_annotation = m_device->CreateAnnotation();
    }

    void Annotation::BeginEvent(std::string_view text) {
        m_annotation->BeginEvent((LPCWSTR)text.data());
    }

    void Annotation::EndEvent() {
        m_annotation->EndEvent();
    }
}
