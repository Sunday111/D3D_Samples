#pragma once

#include <functional>
#include <vector>

#include "EverydayTools/Geom/Vector.h"
#include "Keng/Graphics/FwdDecl.h"
#include "Keng/Core/ISystem.h"

namespace simple_quad_sample
{
    class SimpleModel
    {
    public:
        struct Vertex
        {
            edt::geom::Vector<float, 4> pos;
            edt::geom::Vector<float, 4> col;
        };
        
        struct CB
        {
            edt::geom::Matrix<float, 4, 4> transform;
        };

    public:
        size_t elementsCount = 0;
        keng::graphics::PrimitiveTopology topology;
        keng::graphics::IEffectPtr effect;
        keng::graphics::gpu::IDeviceBufferPtr vertices;
        keng::graphics::gpu::IDeviceBufferPtr constantBuffer;
    };

    class SampleSystem : public keng::core::RefCountImpl<keng::core::ISystem>
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
        // ~ISystem

    protected:
        void MakeFunctionModel(const std::function<float(float)>& function, const SimpleModel::CB& cb,
            float argumentBegin, float argumentRange, size_t samplesCount);

    protected:
        std::vector<SimpleModel> m_models;
        keng::graphics::gpu::ITextureRenderTargetPtr m_textureRT;
        keng::graphics::gpu::IDepthStencilPtr m_depthStencil;
        keng::graphics::gpu::IWindowRenderTargetPtr m_windowRT;
        keng::graphics::gpu::IAnnotationPtr m_annotation;

        keng::resource::IResourceSystemPtr m_resourceSystem;
        keng::graphics::IGraphicsSystemPtr m_graphicsSystem;
        keng::window_system::IWindowSystemPtr m_windowSystem;
    };
}
