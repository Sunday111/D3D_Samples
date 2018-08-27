#pragma once

#include "FwdDecl.h"
#include "Keng/Core/System.h"
#include "Keng/FileSystem/IFileSystem.h"
#include "Keng/Graphics/IGraphicsListener.h"
#include "Keng/Graphics/IGraphicsSystem.h"
#include "Keng/GPU/IGPUSystem.h"
#include "Keng/ResourceSystem/IResourceSystem.h"
#include "Keng/WindowSystem/IWindowSystem.h"

namespace keng::graphics
{
    class GraphicsSystem : public core::System
    <
        IGraphicsSystem, GraphicsSystem,
        keng::filesystem::IFileSystem,
        keng::graphics::gpu::IGPUSystem,
        keng::resource::IResourceSystem,
        keng::window_system::IWindowSystem
    >
    {
    public:
        GraphicsSystem();
        ~GraphicsSystem();

        // ISystem
        virtual void Initialize(const core::IApplicationPtr& app) override;
        virtual bool Update() override;
        virtual void Shutdown() override;

        // IGraphicsSystem
        virtual IDevicePtr GetDevice() override;
        virtual ITexturePtr CreateTexture(const DeviceTextureParameters& params) override;

    private:
        bool m_fullscreen = false;
        core::IApplicationPtr m_app;
        DevicePtr m_device;
    };
}
