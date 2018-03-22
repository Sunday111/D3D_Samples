#pragma once

#include "Keng/WindowSystem/Common.h"

namespace keng::window_system
{
    class IWindowSystem;
    KENG_WINDOW_SYSTEM_API IWindowSystem* CreateSystem();
}
