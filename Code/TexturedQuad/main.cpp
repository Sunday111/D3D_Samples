#include "GraphicsSystem.h"
#include "WindowSystem/WindowSystem.h"
#include "SystemsApp/BaseApplication.h"
#include "Keng/GraphicsSystem.h"
#include "ResourceSystem/ResourceSystem.h"

#include <stdexcept>
#include <sstream>

using namespace textured_quad_sample;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    using TChar = char;
    using WA = WinAPI<TChar>;
    UnusedVar(hPrevInstance, lpCmdLine);
	using Application = BaseApplication;

    try {
        return CallAndRethrow("Main", [&] {
            auto app = std::make_unique<Application>();

			{
				ResourceSystem::CreateParams params {};
				auto sys = std::make_unique<ResourceSystem>();
				sys->Initialize(params, app.get());
				app->AddSystem(std::move(sys));
			}

			{
				WindowSystem::CreateParams params {};
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
				GraphicsSystem::CreateParams params {};

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
        });
    } catch (const std::exception& ex) {
        std::stringstream sstream;
        PrintException(sstream, ex);
        WA::MessageBox_(nullptr, sstream.str().c_str(), "Error", MB_ICONERROR);
    } catch (...) {
        WA::MessageBox_(nullptr, "Unexpected error!", "Error", MB_ICONERROR);
    }
    return 1;
}