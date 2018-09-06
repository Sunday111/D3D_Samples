#pragma once

#include "FwdDecl.h"
#include "Keng/Core/System.h"
#include "Keng/GPU/IGPUSystem.h"
#include "Keng/WindowSystem/IWindowSystem.h"

namespace keng::graphics::gpu
{
    class GPUSystem : public core::System<IGPUSystem, GPUSystem,
        window_system::IWindowSystem>
    {
    public:
        GPUSystem();
        ~GPUSystem();

        // IGPUSystem
        virtual core::Ptr<IDevice> CreateDevice(const DeviceParameters&) override;

    private:
        DevicePtr m_device;
    };
}
