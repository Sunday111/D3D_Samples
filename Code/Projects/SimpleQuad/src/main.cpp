#include "Keng/Core/Application.h"
#include "Keng/WindowSystem/CreateSystem.h"
#include "Keng/WindowSystem/IWindowSystem.h"
#include "Keng/ResourceSystem/CreateSystem.h"
#include "Keng/ResourceSystem/IResourceSystem.h"
#include "Keng/Graphics/GraphicsSystem.h"
#include "GraphicsSystem.h"

#include <stdexcept>
#include <sstream>

#include "Keng/Core/ImplementLauncherInterface.h"

namespace simple_quad_sample
{
    int RunApplicationImpl(HINSTANCE hInstance, HINSTANCE hPrevInstance, char* cmdLine, int nCmdShow) {
        return CallAndRethrowM + [&] {
            UnusedVar(hInstance, hPrevInstance, cmdLine, nCmdShow);
            auto app = std::make_unique<keng::core::Application>();
            app->AddSystem(new GraphicsSystem());
            app->AddSystem(keng::window_system::CreateSystem());
            app->AddSystem(keng::resource::CreateSystem());
            app->Initialize();
            app->Run();
            return 0;
        };
    }
}

MAKE_LAUNCHER_ENTRY_POINT(simple_quad_sample::RunApplicationImpl)
