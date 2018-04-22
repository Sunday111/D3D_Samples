#pragma once

#include "FwdDecl.h"
#include "Keng/ResourceSystem/IResourceFabric.h"

namespace keng::graphics
{
    class ShaderFabric : public core::RefCountImpl<resource::IResourceFabric>
    {
    public:
        virtual const char* GetNodeName() const override;
        virtual const char* GetResourceType() const override;
        virtual resource::IResourcePtr LoadResource(resource::IResourceSystem&, Archive& ar, const resource::IDevicePtr& device) const override;
    };
}