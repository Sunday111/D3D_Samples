#pragma once

#include "BaseApplication.h"
#include "WindowSystem.h"
#include "GraphicsSystem.h"

class Application : public BaseApplication {
public:

    struct CreateParams {
        WindowSystem::CreateParams WindowParams;
        GraphicsSystem::CreateParams GraphicsParams;
    };


    Application(CreateParams& params)
    {
        CallAndRethrow("Application::Application", [&]() {
            Initialize(params);
        });
    }

protected:
    void Initialize(CreateParams& params) {
        CallAndRethrow("Application::Initialize", [&]() {
            InitializeWindowSystem(params);
            params.GraphicsParams.Width = params.WindowParams.Width;
            params.GraphicsParams.Height = params.WindowParams.Height;
            InitializeGraphicsSystem(params);
        });
    }

    void InitializeWindowSystem(CreateParams& params) {
        CallAndRethrow("Application::InitializeWindowSystem", [&]() {
            auto windowSystem = std::make_unique<WindowSystem>(params.WindowParams);
            params.GraphicsParams.hWnd = windowSystem->GetWindow()->GetHandle();
            AddSystem(std::move(windowSystem));
        });
    }

    void InitializeGraphicsSystem(CreateParams& params) {
        CallAndRethrow("Application::InitializeGraphicsSystem", [&]() {
            auto graphicsSystem = std::make_unique<GraphicsSystem>(params.GraphicsParams);
            AddSystem(std::move(graphicsSystem));
        });
    }

private:
};