#pragma once

#include "Keng/Core/IRefCountObject.h"
#include "Keng/GraphicsCommon/ShaderType.h"

namespace keng::graphics::gpu
{
    class ISampler : public core::IRefCountObject
    {
    public:
        virtual void AssignToPipeline(ShaderType shaderType, size_t index) = 0;
    };
}
