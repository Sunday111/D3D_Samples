#pragma once

#include "Keng/Core/IRefCountObject.h"
#include "Keng/GraphicsCommon/FwdDecl.h"

namespace keng::graphics::gpu
{
    class ISampler : public core::IRefCountObject
    {
    public:
        virtual void AssignToPipeline(const ShaderType& shaderType, size_t index) = 0;
    };
}
