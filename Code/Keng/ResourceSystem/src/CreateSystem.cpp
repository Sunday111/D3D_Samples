#include "Keng/ResourceSystem/CreateSystem.h"
#include "ResourceSystem.h"

namespace keng::resource
{
    IResourceSystem* CreateSystem()
    {
        return new ResourceSystem();
    }
}
