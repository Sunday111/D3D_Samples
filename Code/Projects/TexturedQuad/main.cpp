#include "GraphicsSystem.h"
#include "WindowSystem/WindowSystem.h"
#include "SystemsApp/BaseApplication.h"
#include "Keng/GraphicsSystem.h"
#include "ResourceSystem/ResourceSystem.h"

#include <stdexcept>
#include <sstream>

#include "Keng/ImplementLauncherInterface.h"

namespace textured_quad_sample {
    int RunApplicationImpl(HINSTANCE hInstance, HINSTANCE hPrevInstance, char* cmdLine, int nCmdShow) {
		UnusedVar(hPrevInstance, cmdLine);
        using Application = BaseApplication;
        return CallAndRethrowM + [&] {
            auto app = std::make_unique<Application>();

            {
                ResourceSystem::CreateParams params{};
                auto sys = std::make_unique<ResourceSystem>();
                sys->Initialize(params, app.get());
                app->AddSystem(std::move(sys));
            }

            {
                WindowSystem::CreateParams params{};
                params.Height = 720;
                params.Width = 1280;
                params.hInstance = hInstance;
                params.nCmdShow = nCmdShow;
                params.WindowTitle = "Textured Quad";

                auto sys = std::make_unique<WindowSystem>();
                sys->Initialize(params, app.get());
                app->AddSystem(std::move(sys));
            }

            {
                GraphicsSystem::CreateParams params{};

                #ifdef _DEBUG
                params.noDeviceMultithreading = true;
                params.debugDevice = true;
                #endif

                auto sys = std::make_unique<GraphicsSystem>();
                sys->Initialize(params, app.get());
                app->AddSystem(std::move(sys));
            }

            app->Run();

            return 0;
        };
    }
}

MAKE_LAUNCHER_ENTRY_POINT(textured_quad_sample::RunApplicationImpl)
