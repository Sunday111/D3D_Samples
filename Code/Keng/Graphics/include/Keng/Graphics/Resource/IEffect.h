#pragma once

#include "Keng/ResourceSystem/IResource.h"

namespace keng::graphics
{
    class IEffect : public resource::IResource
    {
    public:
        virtual void Use() = 0;
        virtual void InitDefaultInputLayout() = 0;
        virtual ~IEffect() = default;
    };
}

