#include "Keng/Core/Application.h"
#include "Keng/Core/Systems/WindowSystem/WindowSystem.h"
#include "Keng/Core/Systems/ResourceSystem/ResourceSystem.h"
#include "Keng/Graphics/GraphicsSystem.h"
#include "GraphicsSystem.h"

#include <stdexcept>
#include <sstream>

#include "Keng/Core/ImplementLauncherInterface.h"

namespace textured_quad_sample {
    int RunApplicationImpl(HINSTANCE hInstance, HINSTANCE hPrevInstance, char* cmdLine, int nCmdShow) {
		UnusedVar(hPrevInstance, cmdLine);
		return CallAndRethrowM + [&] {
			UnusedVar(hInstance, hPrevInstance, cmdLine, nCmdShow);
			auto app = std::make_unique<keng::Application>();
			app->AddSystem(std::make_unique<keng::ResourceSystem>());
			app->AddSystem(std::make_unique<keng::WindowSystem>());
			app->AddSystem(std::make_unique<GraphicsSystem>());
			app->Initialize();
			app->Run();
			return 0;
        };
    }
}

MAKE_LAUNCHER_ENTRY_POINT(textured_quad_sample::RunApplicationImpl)
