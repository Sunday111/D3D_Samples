#pragma once

#include "Keng/Core/IRefCountObject.h"
#include "Keng/Core/Ptr.h"

namespace keng::resource
{
    class IResource : public core::IRefCountObject
    {
    public:
        virtual ~IResource() = default;
    };

}