#pragma once

namespace keng::resource
{
    class IResourceSystem;
}

namespace keng::graphics
{
    class ResourceFabricRegisterer
    {
    public:
        void Register(resource::IResourceSystem* resourceSystem);
    };
}