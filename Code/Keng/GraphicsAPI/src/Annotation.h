#pragma once

#include "FwdDecl.h"
#include "Keng/GraphicsAPI/IAnnotation.h"
#include "WinWrappers/ComPtr.h"
#include "d3d11_1.h"

namespace keng::graphics_api
{
    class Annotation : public core::RefCountImpl<IAnnotation>
    {
    public:
        Annotation(Device& device);
        virtual void BeginEvent(const wchar_t*) override;
        virtual void EndEvent() override;

    private:
        DevicePtr m_device;
        ComPtr<ID3DUserDefinedAnnotation> m_annotation;
    };
}
