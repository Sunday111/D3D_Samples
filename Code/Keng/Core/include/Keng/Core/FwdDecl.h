#pragma once

#include "Keng/Core/IRefCountObject.h"
#include "Keng/Core/Ptr.h"

namespace keng::core
{
    class ISystem;
    using ISystemPtr = core::Ptr<ISystem>;

    class IApplication;
    using IApplicationPtr = core::Ptr<IApplication>;
}
