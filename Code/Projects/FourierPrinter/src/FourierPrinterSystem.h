#pragma once

#include <functional>
#include <vector>

#include "EverydayTools/Geom/Vector.h"

#include "Keng/Core/System.h"
#include "Keng/Graphics/IGraphicsSystem.h"
#include "Keng/ResourceSystem/IResourceSystem.h"
#include "Keng/WindowSystem/IWindowSystem.h"

namespace simple_quad_sample
{
    using v3f = edt::geom::Vector<float, 3>;
    using v4f = edt::geom::Vector<float, 4>;
    using m4f = edt::geom::Matrix<float, 4, 4>;

    class SimpleModel
    {
    public:
        struct Vertex
        {
            v4f pos;
            v4f col;
        };
        
        struct CB
        {
            m4f transform;
        };

    public:
        size_t elementsCount = 0;
        keng::graphics::PrimitiveTopology topology;
        keng::graphics::IEffectPtr effect;
        keng::graphics::gpu::IDeviceBufferPtr vertices;
        keng::graphics::gpu::IDeviceBufferPtr constantBuffer;
    };

    class FourierPrinterSystem :
        public keng::core::System<
            keng::core::ISystem, FourierPrinterSystem,
            keng::resource::IResourceSystem,
            keng::graphics::IGraphicsSystem,
            keng::window_system::IWindowSystem>
    {
    public:
        FourierPrinterSystem();
        ~FourierPrinterSystem();

        // ISystem
        static const char* SystemName() { return "FourierPrinter"; }
        virtual void OnSystemEvent(const keng::core::IApplicationPtr&, const keng::core::SystemEvent& e) override;
        // ~ISystem

    protected:
        void MakeFunctionModel(const std::function<float(float)>& function, const SimpleModel::CB& cb,
            float argumentBegin, float argumentRange, size_t samplesCount, const v4f& color);

    private:
        void Initialize(const keng::core::IApplicationPtr& app);
        bool Update();

    private:
        std::vector<SimpleModel> m_models;
        keng::graphics::gpu::ITextureRenderTargetPtr m_textureRT;
        keng::graphics::gpu::IDepthStencilPtr m_depthStencil;
        keng::graphics::gpu::IWindowRenderTargetPtr m_windowRT;
        keng::graphics::gpu::IAnnotationPtr m_annotation;
    };
}
