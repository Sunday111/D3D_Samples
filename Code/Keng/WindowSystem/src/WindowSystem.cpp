#include "WindowSystem.h"
#include "Keng/Base/Serialization/SerializeMandatory.h"
#include "keng/Base/Serialization/ReadFileToBuffer.h"
#include "keng/Base/Serialization/OpenArchiveJSON.h"
#include "yasli/JSONIArchive.h"
#include "yasli/STL.h"

namespace keng::window_system
{
    void WindowSystem::Initialize(core::IApplication* app) {
        CallAndRethrowM + [&] {
            UnusedVar(app);
            auto params = ReadDefaultParams();
            m_windowClass = std::make_unique<MainWindowClass<TChar>>(params.hInstance);
            m_window = m_windowClass->MakeWindow(params.WindowTitle.data());
            m_window->Show(params.nCmdShow);
            m_window->SetWindowClientSize(params.Width, params.Height);
            m_window->Update();
        };
    }

    bool WindowSystem::Update() {
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

    MainWindow<WindowSystem::TChar>* WindowSystem::GetWindow() const {
        return m_window.get();
    }

    void WindowSystem::SystemParams::serialize(Archive& ar) {
        ar(Width, "width");
        ar(Height, "height");
        ar(WindowTitle, "title");
    }

    WindowSystem::SystemParams WindowSystem::ReadDefaultParams() {
        return CallAndRethrowM + [&] {
            struct File {
                void serialize(Archive& ar) {
                    SerializeMandatory(ar, params, "window_system");
                }
                SystemParams params;
            };

            File file {};
            file.params.hInstance = GetModuleHandle(NULL);
            file.params.WindowTitle = "MainWindow";

            try {
                auto buffer = ReadFileToBuffer("Configs/window_system.json");
                yasli::JSONIArchive configArchive;
                OpenArchiveJSON(buffer, configArchive);
                configArchive(file);
            } catch (...) {
            }

            return file.params;
        };
    }

    bool WindowSystem::ForEachSystemDependency(bool(*pfn)(const char* systemGUID, void* context), void* context) {
        return CallAndRethrowM + [&]() -> bool {
            for (auto& guid : m_dependencies) {
                if (pfn(guid.data(), context)) {
                    return true;
                }
            }

            return false;
        };
    }

    const char* WindowSystem::GetSystemGUID() {
        return GetGUID();
    }

    IWindow* WindowSystem::GetWindow() {
        return m_window.get();
    }
}
