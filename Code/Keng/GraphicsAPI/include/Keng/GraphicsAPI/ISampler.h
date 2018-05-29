#pragma once

#include "Keng/Core/IRefCountObject.h"
#include "Keng/Graphics/ShaderType.h"

namespace keng::graphics_api
{
    class ISampler : public core::IRefCountObject
    {
    public:
        virtual void AssignToPipeline(ShaderType shaderType, size_t index) = 0;
    };
}
