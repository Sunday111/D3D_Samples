#pragma once

#include "FwdDecl.h"

namespace keng::graphics
{
    class ResourceFabricRegisterer
    {
    public:
        void Register(resource::IResourceSystem* resourceSystem);
    };
}