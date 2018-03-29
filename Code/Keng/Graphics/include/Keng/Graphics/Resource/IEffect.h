#pragma once

#include "Keng/ResourceSystem/IResource.h"

namespace keng::graphics
{
    class IEffect : public resource::IResource
    {
    public:
        virtual void AssignToPipeline() = 0;
        virtual void InitDefaultInputLayout() = 0;
        virtual ~IEffect() = default;
    };
}

