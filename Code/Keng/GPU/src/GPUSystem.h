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
        virtual IDevicePtr GetDevice() override;
        virtual ISamplerPtr CreateSampler(const SamplerParameters& params) override;
        virtual void Draw(size_t vertices, size_t offset) override;
        virtual void SetViewport(const ViewportParameters&) override;
        virtual IAnnotationPtr CreateAnnotation() override;

    private:
        bool m_fullscreen = false;
        core::IApplicationPtr m_app;
        core::Ptr<Device> m_device;
    };
}
