#pragma once

namespace keng::resource
{
    class IResourceSystem;
}

namespace keng::graphics
{
    class GraphicsSystem;

    class ResourceFabricRegisterer
    {
    public:
        ResourceFabricRegisterer(GraphicsSystem* graphicsSystem);
        void Register(resource::IResourceSystem* resourceSystem);

    private:
        GraphicsSystem* m_graphicsSystem = nullptr;
    };
}