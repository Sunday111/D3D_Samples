#include "Keng/Core/Ptr.h"

namespace keng::resource
{
    class IResource;
    using IResourcePtr = core::Ptr<IResource>;

    class IDevice;
    using IDevicePtr = core::Ptr<IDevice>;

    class IResourceFabric;
    using IResourceFabricPtr = core::Ptr<IResourceFabric>;

    class IResourceSystem;
}
