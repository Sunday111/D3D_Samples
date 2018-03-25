#include "Keng/ResourceSystem/CreateSystem.h"
#include "Keng/ResourceSystem/IResourceFabric.h"
#include "ResourceSystem.h"

namespace keng::resource
{
    IResourceSystem* CreateSystem()
    {
        return new ResourceSystem();
    }
}
