#include "Keng/Core/Ptr.h"

namespace keng::resource
{
    class IResource;
    using ResourcePtr = core::Ptr<IResource>;

    class IDevice;
    using DevicePtr = core::Ptr<IDevice>;

    class IResourceFabric;
    using ResourceFabricPtr = core::Ptr<IResourceFabric>;
}
