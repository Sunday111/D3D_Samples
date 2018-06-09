#pragma once

#include "Keng/GPU/FwdDecl.h"
#include "Keng/GraphicsCommon/PrimitiveTopology.h"
#include "Keng/GraphicsCommon/ShaderType.h"
#include "Keng/Core/ISystem.h"
#include "EverydayTools/Array/ArrayView.h"

namespace keng::graphics::gpu
{
    class IGPUSystem : public core::ISystem
    {
    public:
        static const char* SystemName() { return "KengGPU"; }
        virtual core::Ptr<IDevice> CreateDevice(const DeviceParameters&) = 0;
    };
}
