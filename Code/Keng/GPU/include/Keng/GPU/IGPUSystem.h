#pragma once

#include "Keng/GPU/FwdDecl.h"
#include "Keng/Core/ISystem.h"

namespace keng::graphics::gpu
{
    class IGPUSystem : public core::ISystem
    {
    public:
        static const char* SystemName() { return "KengGPU"; }
        virtual core::Ptr<IDevice> CreateDevice(const DeviceParameters&) = 0;
    };
}
