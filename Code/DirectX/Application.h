#pragma once

#include <thread>

#include "SystemsApp/BaseApplication.h"
#include "WindowSystem.h"
#include "GraphicsSystem.h"
#include "ResourceSystem/ResourceSystem.h"
#include "WinWrappers/WinWrappers.h"

class Application :
    public BaseApplication
{
public:

    struct CreateParams {
        HINSTANCE hInstance = nullptr;
        const char* WindowTitle = nullptr;
        int nCmdShow = 0;
        unsigned Width = 1280;
        unsigned Height = 720;
        HWND hWnd = nullptr;
        bool debugDevice = false;
        bool noDeviceMulithreading = false;
        bool VSync = true;
    };

    Application(CreateParams& params) :
        BaseApplication(params.VSync)
    {
        CallAndRethrowM + [&]{
            Initialize(params);
        };
    }

    ResourceSystem* GetResourceSystem() const {
        return m_resourceSystem;
    }

    WindowSystem* GetWindowSystem() const {
        return m_windowSystem;
    }

    GraphicsSystem* GetGraphicsSystem() const {
        return m_graphicsSystem;
    }

protected:
    void Initialize(CreateParams& params) {
        CallAndRethrowM + [&]{
            InitializeResourceSystem();
            InitializeWindowSystem(params);
            InitializeGraphicsSystem(params);
        };
    }

    void InitializeResourceSystem() {
        CallAndRethrowM + [&]{
            m_resourceSystem = static_cast<ResourceSystem*>(AddSystem(std::make_unique<ResourceSystem>()));
        };
    }

    void InitializeWindowSystem(CreateParams& params) {
        CallAndRethrowM + [&]{
            WindowSystem::CreateParams p;
            p.Width = params.Width;
            p.Height = params.Height;
            p.hInstance = params.hInstance;
            p.nCmdShow = params.nCmdShow;
            p.WindowTitle = params.WindowTitle;
            m_windowSystem = static_cast<WindowSystem*>(AddSystem(std::make_unique<WindowSystem>(p)));
        };
    }

    void InitializeGraphicsSystem(CreateParams& params) {
        CallAndRethrowM + [&]{
            GraphicsSystem::CreateParams p;
            p.Width = params.Width;
            p.Height = params.Height;
            p.debugDevice = params.debugDevice;
            p.noDeviceMultithreading = params.noDeviceMulithreading;
            p.hWnd = m_windowSystem->GetWindow()->GetHandle();
            m_graphicsSystem = static_cast<GraphicsSystem*>(AddSystem(std::make_unique<GraphicsSystem>(this, p)));
        };
    }

private:
    ResourceSystem* m_resourceSystem = nullptr;
    WindowSystem* m_windowSystem = nullptr;
    GraphicsSystem* m_graphicsSystem = nullptr;
};