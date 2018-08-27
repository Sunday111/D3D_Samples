#pragma once

#include "Keng/Graphics/FwdDecl.h"
#include "Keng/Core/DependencyStorage.h"
#include "Keng/Core/ISystem.h"

namespace simple_quad_sample
{
    class SampleSystem :
        public keng::core::RefCountImpl<keng::core::ISystem>,
        public keng::core::DependenciesContainer<
            keng::resource::IResourceSystem,
            keng::graphics::IGraphicsSystem,
            keng::window_system::IWindowSystem>
    {
    public:
        SampleSystem();
        ~SampleSystem();

        // ISystem
        virtual void Initialize(const keng::core::IApplicationPtr& app) override;
        virtual bool Update() override;
        virtual void Shutdown() override;
        virtual const char* GetSystemName() const override;
        virtual bool ForEachDependency(const edt::Delegate<bool(const char*)>& delegate) const override;

    protected:
        keng::graphics::IEffectPtr m_effect;
        keng::graphics::gpu::IDeviceBufferPtr m_constantBuffer;
        keng::graphics::gpu::IDeviceBufferPtr m_vertexBuffer;
        keng::graphics::gpu::ITextureRenderTargetPtr m_textureRT;
        keng::graphics::gpu::IDepthStencilPtr m_depthStencil;
        keng::graphics::gpu::IWindowRenderTargetPtr m_windowRT;
        keng::graphics::gpu::IAnnotationPtr m_annotation;
    };
}
