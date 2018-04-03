#pragma once

#include "Keng/Core/FwdDecl.h"

namespace keng::window_system
{
    class IWindow;
    class IWindowListener;

    class IWindowSystem;
    using IWindowSystemPtr = core::Ptr<IWindowSystem>;
}