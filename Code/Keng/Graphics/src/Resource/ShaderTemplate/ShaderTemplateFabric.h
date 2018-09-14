#pragma once

#include "FwdDecl.h"
#include "Keng/ResourceSystem/IResourceFabric.h"

namespace keng::graphics
{
    class ShaderTemplateFabric :
        public core::RefCountImpl<resource::IResourceFabric>
    {
    public:
        virtual const char* GetNodeName() const override final;
        virtual const char* GetResourceType() const override final;
        virtual resource::IResourcePtr LoadResource(resource::IResourceSystem&, Archive& ar, const resource::IDevicePtr& device) const override final;
    };
}
