#pragma once

#include "Keng/Core/IRefCountObject.h"
#include <string_view>

namespace keng::graphics
{
    class IAnnotation : public core::IRefCountObject
    {
    public:
        virtual void BeginEvent(std::basic_string_view<wchar_t> text) = 0;
        virtual void EndEvent() = 0;
    };
}
