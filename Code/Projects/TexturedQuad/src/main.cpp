#include "Keng/Core/Application.h"
#include "Keng/Core/Systems/WindowSystem/WindowSystem.h"
#include "Keng/ResourceSystem/IResourceSystem.h"
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
			auto app = std::make_unique<keng::core::Application>();
            app->AddSystem(new GraphicsSystem());
            app->AddSystem(new keng::core::WindowSystem());
            app->AddSystem(keng::resource::CreateResourceSystem());
			app->Initialize();
			app->Run();
			return 0;
        };
    }
}

MAKE_LAUNCHER_ENTRY_POINT(textured_quad_sample::RunApplicationImpl)
