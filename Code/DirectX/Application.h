#pragma once

#include <thread>

#include "BaseApplication.h"
#include "FrameRateCounter.h"
#include "WindowSystem.h"
#include "GraphicsSystem.h"
#include "ResourceSystem.h"

class Application :
    public BaseApplication
{
public:
    using Period = std::chrono::nanoseconds;
    static constexpr auto FpsSamplesCount = 100;
    static constexpr auto DesiredFPS = 60;
    using TFrameRateCounter = FrameRateCounter<Period, FpsSamplesCount>;

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
        return CallAndRethrow("Application::Update", [&]() {
            if (m_vSync) {
                using namespace std::chrono;
                auto t0 = high_resolution_clock::now();
                auto retVal = BaseApplication::Update();
                auto t1 = high_resolution_clock::now();
                auto average = m_fpsCounter.CalcAverageTick(t1 - t0);
                auto sleep = TFrameRateCounter::GetDesiredFrameDuration(DesiredFPS) - average;
                if (sleep.count() > 0) {
                    //std::this_thread::sleep_for(sleep);
                    Sleep((DWORD)duration_cast<milliseconds>(sleep).count());
                }
                return retVal;
            } else {
                return BaseApplication::Update();
            }
        });
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

    TFrameRateCounter m_fpsCounter;
};