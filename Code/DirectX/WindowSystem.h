#pragma once

#include "MainWindow.h"
#include "WinWrappers/WinWrappers.h"
#include "SystemsApp/ISystem.h"

class WindowSystem : public ISystem {
public:
    using TChar = char;
    using WA = WinAPI<TChar>;

    struct CreateParams {
        HINSTANCE hInstance = nullptr;
        const char* WindowTitle = nullptr;
        int nCmdShow = 0;
        unsigned Width = 1280;
        unsigned Height = 720;
    };

    WindowSystem(CreateParams params) {
        CallAndRethrowM + [&] {
            m_windowClass = std::make_unique<MainWindowClass<TChar>>(params.hInstance);
            m_window = m_windowClass->MakeWindow(params.WindowTitle);
            m_window->Show(params.nCmdShow);
            m_window->SetWindowClientSize(params.Width, params.Height);
            m_window->Update();
        };
    }

    bool Update(IApplication*) override {
        return CallAndRethrowM + [&] {
            MSG msg;
            if (WA::PeekMessage_(&msg, nullptr, 0, 0, PM_REMOVE)) {
                // Handle windows messages
                TranslateMessage(&msg);
                WA::DispatchMessage_(&msg);
                if (msg.message == WM_QUIT) {
                    return false;
                }
            }
            return true;
        };
    }

    MainWindow<TChar>* GetWindow() const {
        return m_window.get();
    }

private:
    std::unique_ptr<MainWindowClass<TChar>> m_windowClass;
    std::unique_ptr<MainWindow<TChar>> m_window;
};