#pragma once

#include "Keng/ResourceSystem/IResource.h"

namespace keng::graphics
{
    class IDevice;

    class IEffect : public resource::IResource
    {
    public:
        virtual void Use(IDevice* device) = 0;
        virtual void InitDefaultInputLayout(IDevice* device) = 0;
        virtual ~IEffect() = default;
    };
}

