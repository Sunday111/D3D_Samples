#pragma once

#include "Keng/Core/System.h"
#include "Keng/Graphics/IGraphicsSystem.h"
#include "Keng/WindowSystem/IWindowSystem.h"
#include "Keng/ResourceSystem/IResourceSystem.h"

namespace project_ecs
{
    class ProjectECS : public keng::core::System<
            keng::core::ISystem, ProjectECS,
            keng::resource::IResourceSystem,
            keng::graphics::IGraphicsSystem,
            keng::window_system::IWindowSystem>
    {
    public:
        ProjectECS();
        ~ProjectECS();

        // ISystem
        static const char* SystemName() { return "ProjectECS"; }
        virtual void OnSystemEvent(const keng::core::IApplicationPtr&, const keng::core::SystemEvent& e) override final;

    private:
        void Initialize(const keng::core::IApplicationPtr& app);
        bool Update();
    };
}
