#pragma once

#include "BaseApplication.h"
#include "WindowSystem.h"
#include "GraphicsSystem.h"
#include "ResourceSystem.h"
#include <chrono>

float CalcAverageTick(long long newtick);

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
        m_vSync(params.VSync)
    {
        CallAndRethrow("Application::Application", [&]() {
            Initialize(params);
        });
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

    bool Update() override {
        if (m_vSync) {
            auto t0 = std::chrono::high_resolution_clock::now();
            auto retVal = BaseApplication::Update();
            auto t1 = std::chrono::high_resolution_clock::now();
            auto dt = std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0);
            auto average = CalcAverageTick(dt.count());
            auto sleep = (decltype(average))(1000000000.f / 60.f) - average;
            if (sleep > 0) {
                Sleep((DWORD)(sleep / 1000000));
            }
            return retVal;
        } else {
            return BaseApplication::Update();
        }
    }

protected:
    void Initialize(CreateParams& params) {
        CallAndRethrow("Application::Initialize", [&]() {
            InitializeResourceSystem();
            InitializeWindowSystem(params);
            InitializeGraphicsSystem(params);
        });
    }

    void InitializeResourceSystem() {
        CallAndRethrow("Application::InitializeResourceSystem", [&]() {
            m_resourceSystem = static_cast<ResourceSystem*>(AddSystem(std::make_unique<ResourceSystem>()));
        });
    }

    void InitializeWindowSystem(CreateParams& params) {
        CallAndRethrow("Application::InitializeWindowSystem", [&]() {
            WindowSystem::CreateParams p;
            p.Width = params.Width;
            p.Height = params.Height;
            p.hInstance = params.hInstance;
            p.nCmdShow = params.nCmdShow;
            p.WindowTitle = params.WindowTitle;
            m_windowSystem = static_cast<WindowSystem*>(AddSystem(std::make_unique<WindowSystem>(p)));
        });
    }

    void InitializeGraphicsSystem(CreateParams& params) {
        CallAndRethrow("Application::InitializeGraphicsSystem", [&]() {
            GraphicsSystem::CreateParams p;
            p.Width = params.Width;
            p.Height = params.Height;
            p.debugDevice = params.debugDevice;
            p.noDeviceMultithreading = params.noDeviceMulithreading;
            p.hWnd = m_windowSystem->GetWindow()->GetHandle();
            m_graphicsSystem = static_cast<GraphicsSystem*>(AddSystem(std::make_unique<GraphicsSystem>(this, p)));
        });
    }

private:
    bool m_vSync = true;
    ResourceSystem* m_resourceSystem = nullptr;
    WindowSystem* m_windowSystem = nullptr;
    GraphicsSystem* m_graphicsSystem = nullptr;
};