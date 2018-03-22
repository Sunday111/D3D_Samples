#pragma once

#include "Keng/ResourceSystem/Common.h"

namespace keng::resource
{
    class IResourceSystem;
    KENG_RESOURCE_SYSTEM_API IResourceSystem* CreateSystem();
}
