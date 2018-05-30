#pragma once

#include "Keng/Core/IRefCountObject.h"

namespace keng::gpu
{
    class IAnnotation : public core::IRefCountObject
    {
    public:
        virtual void BeginEvent(const wchar_t* text) = 0;
        virtual void EndEvent() = 0;
    };
}
