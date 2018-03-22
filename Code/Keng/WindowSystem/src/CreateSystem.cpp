#include "Keng/WindowSystem/CreateSystem.h"
#include "WindowSystem.h"

namespace keng::window_system
{
    IWindowSystem* CreateSystem() {
        return new WindowSystem();
    }
}
