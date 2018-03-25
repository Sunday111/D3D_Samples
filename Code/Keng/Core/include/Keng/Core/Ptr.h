#pragma once

#include "EverydayTools/Pointer/IntrusivePtr.h"
#include "Keng/Core/IRefCountObject.h"
#include <type_traits>

namespace keng::core
{
    class PtrTraits
    {
    public:
        static int AddReference(IRefCountObject* ptr) {
            return ptr->AddReference();
        }

        static int ReleaseReference(IRefCountObject* ptr) {
            return ptr->RemoveReference();
        }
    };

    template<typename T>
    using Ptr = edt::IntrusivePtr<T, PtrTraits>;
}
