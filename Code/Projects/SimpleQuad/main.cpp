#include "Keng/Application.h"
#include "Keng/Systems/WindowSystem/WindowSystem.h"
#include "Keng/Systems/GraphicsSystem/GraphicsSystem.h"
#include "Keng/Systems/ResourceSystem/ResourceSystem.h"
#include "GraphicsSystem.h"

#include <stdexcept>
#include <sstream>

#include "Keng/ImplementLauncherInterface.h"

namespace simple_quad_sample {
    int RunApplicationImpl(HINSTANCE hInstance, HINSTANCE hPrevInstance, char* cmdLine, int nCmdShow) {
		return CallAndRethrowM + [&] {
			UnusedVar(hInstance, hPrevInstance, cmdLine, nCmdShow);
			auto app = std::make_unique<keng::Application>();
			app->AddSystem(std::make_unique<GraphicsSystem>());
			app->AddSystem(std::make_unique<keng::WindowSystem>());
			app->AddSystem(std::make_unique<keng::ResourceSystem>());
            app->Initialize();
            app->Run();
            return 0;
        };
    }
}

MAKE_LAUNCHER_ENTRY_POINT(simple_quad_sample::RunApplicationImpl)
