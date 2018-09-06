#pragma once

#include "Keng/Core/Common.h"
#include "Keng/Core/IRefCountObject.h"
#include "Keng/Core/Ptr.h"

namespace keng::core
{
    class ISystem;
    using ISystemPtr = core::Ptr<ISystem>;

    class IGlobalSystem;
    using IGlobalSystemPtr = core::Ptr<IGlobalSystem>;

    class IApplication;
    using IApplicationPtr = core::Ptr<IApplication>;

    enum class SystemEvent;
}
