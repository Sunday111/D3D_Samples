#pragma once

#include "FwdDecl.h"
#include "Keng/FileSystem/FwdDecl.h"
#include "Keng/Graphics/IGraphicsListener.h"
#include "Keng/GraphicsCommon/PrimitiveTopology.h"
#include "Keng/Graphics/IGraphicsSystem.h"
#include "Keng/GPU/IGPUSystem.h"
#include "Keng/GPU/IDevice.h"

namespace keng::graphics
{
    class GraphicsSystem : public core::RefCountImpl<IGraphicsSystem>
    {
    public:
        GraphicsSystem();
        ~GraphicsSystem();

        // ISystem
        virtual const char* GetSystemName() const override;
        virtual bool ForEachDependency(const edt::Delegate<bool(const char*)>& delegate) const override;
        virtual void Initialize(const core::IApplicationPtr& app) override;
        virtual bool Update() override;
        virtual void Shutdown() override;

        // IGraphicsSystem
        virtual IDevicePtr GetDevice() override;
        virtual ITexturePtr CreateTexture(const gpu::TextureParameters& params) override;

    private:
        bool m_fullscreen = false;
        core::IApplicationPtr m_app;
        DevicePtr m_device;

        resource::IResourceSystemPtr m_resourceSystem;
        gpu::IGPUSystemPtr m_api;
    };
}
