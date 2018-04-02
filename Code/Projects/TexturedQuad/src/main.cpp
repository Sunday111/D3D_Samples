#include "Keng/Core/Application.h"
#include "Keng/WindowSystem/CreateSystem.h"
#include "Keng/WindowSystem/IWindowSystem.h"
#include "Keng/ResourceSystem/CreateSystem.h"
#include "Keng/ResourceSystem/IResourceSystem.h"
#include "Keng/Graphics/IGraphicsSystem.h"
#include "SampleSystem.h"
#include "Keng/Graphics/CreateSystem.h"

#include <stdexcept>
#include <sstream>

#include "Keng/Core/ImplementLauncherInterface.h"

namespace textured_quad_sample
{
    int RunApplicationImpl(HINSTANCE hInstance, HINSTANCE hPrevInstance, char* cmdLine, int nCmdShow) {
        return CallAndRethrowM + [&] {
            UnusedVar(hInstance, hPrevInstance, cmdLine, nCmdShow);
            auto app = std::make_unique<keng::core::Application>();
            app->AddSystem(keng::graphics::CreateSystem());
            app->AddSystem(keng::window_system::CreateSystem());
            app->AddSystem(keng::resource::CreateSystem());
            app->AddSystem(new SampleSystem());
            app->Initialize();
            app->Run();
            return 0;
        };
    }
}

MAKE_LAUNCHER_ENTRY_POINT(textured_quad_sample::RunApplicationImpl)
