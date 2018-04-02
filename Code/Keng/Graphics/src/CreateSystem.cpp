#include "Keng/Graphics/CreateSystem.h"
#include "GraphicsSystem.h"

namespace keng::graphics
{
    KENG_GRAPHICS_SYSTEM_API IGraphicsSystem* CreateSystem() {
        return new GraphicsSystem();
    }
}
