#pragma once

#include "Keng/Graphics/FwdDecl.h"
#include "Keng/Graphics/IAnnotation.h"

namespace keng::graphics
{
    class ScopedAnnotation
    {
    public:
        ScopedAnnotation(const IAnnotationPtr& annotation, std::basic_string_view<wchar_t> text) :
            m_annotation(annotation)
        {
            m_annotation->BeginEvent(text);
        }

        ~ScopedAnnotation() {
            m_annotation->EndEvent();
        }

    private:
        IAnnotationPtr m_annotation;
    };

    template<typename F>
    inline decltype(auto) Annotate(const IAnnotationPtr& annotation, std::basic_string_view<wchar_t> text, F&& f) {
        ScopedAnnotation scopedAnnotation(annotation, text);
        return static_cast<decltype(f.operator()())>(f());
    }
}
