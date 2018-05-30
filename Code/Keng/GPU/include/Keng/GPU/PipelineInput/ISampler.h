#pragma once

#include "Keng/Core/IRefCountObject.h"
#include "Keng/GPU/Shader/ShaderType.h"

namespace keng::gpu
{
    class ISampler : public core::IRefCountObject
    {
    public:
        virtual void AssignToPipeline(ShaderType shaderType, size_t index) = 0;
    };
}
