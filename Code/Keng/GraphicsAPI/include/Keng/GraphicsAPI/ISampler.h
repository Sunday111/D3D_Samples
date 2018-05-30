#pragma once

#include "Keng/Core/IRefCountObject.h"
#include "Keng/GraphicsAPI/Shader/ShaderType.h"

namespace keng::graphics_api
{
    class ISampler : public core::IRefCountObject
    {
    public:
        virtual void AssignToPipeline(ShaderType shaderType, size_t index) = 0;
    };
}
