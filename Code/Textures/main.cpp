#include <stdexcept>
#include <sstream>

#include "Application.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    using TChar = char;
    using WA = WinAPI<TChar>;
    UnusedVar(hPrevInstance, lpCmdLine);
    try {
        return CallAndRethrow("Main", [&] {
            std::unique_ptr<Application> app;

            {
                Application::CreateParams params;
                params.hInstance = hInstance;
                params.nCmdShow = nCmdShow;
                params.WindowTitle = "Main Window";
                params.Width = 1280;
                params.Height = 720;
                params.VSync = true;
                params.noDeviceMulithreading = false;

                #ifdef _DEBUG
                params.debugDevice = true;
                #endif

                app = std::make_unique<Application>(params);
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