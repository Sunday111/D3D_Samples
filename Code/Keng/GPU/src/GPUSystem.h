#pragma once

#include "FwdDecl.h"
#include "Keng/FileSystem/FwdDecl.h"
#include "Keng/GPU/PrimitiveTopology.h"
#include "Keng/GPU/IGPUSystem.h"
#include "Device.h"

namespace keng::gpu
{
    class GPUSystem : public core::RefCountImpl<IGPUSystem>
    {
    public:
        GPUSystem();
        ~GPUSystem();

        // ISystem
        virtual const char* GetSystemName() const override;
        virtual bool ForEachDependency(const edt::Delegate<bool(const char*)>& delegate) const override;
        virtual void Initialize(const core::IApplicationPtr& app) override;
        virtual bool Update() override;
        virtual void Shutdown() override;

        // IGPUSystem
        virtual core::Ptr<IDevice> CreateDevice(const DeviceParameters&) override;

    private:
        core::Ptr<Device> m_device;
    };
}
