#include "Keng/Core/Systems/WindowSystem/WindowSystem.h"
#include "Xml.h"

namespace keng
{
	const char* WindowSystem::GetGUID() {
		return "4CD74D12-0159-44EC-B79D-9D1422221A5C";
	}

    void WindowSystem::Initialize(IApplication* app) {
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

    WindowSystem::SystemParams WindowSystem::ReadDefaultParams() {
        return CallAndRethrowM + [&] {
            SystemParams params{};
            params.hInstance = GetModuleHandle(NULL);
            params.WindowTitle = "MainWindow";

            try {
                XmlDocument configDoc("Configs/window_system.xml");
                auto systemNode = configDoc.GetFirstNode("window_system");
                if (auto windowNode = systemNode->FindFirstNode("window")) {
                    if (auto widthNode = windowNode->FindFirstNode("width")) {
                        auto widthNodeText = widthNode->GetValue();
                        params.Width = std::stoi(widthNodeText.data());
                    }

                    if (auto heightNode = windowNode->FindFirstNode("height")) {
                        auto heightNodeText = heightNode->GetValue();
                        params.Height = std::stoi(heightNodeText.data());
                    }

                    if (auto titleNode = windowNode->FindFirstNode("title")) {
                        params.WindowTitle = titleNode->GetValue();
                    }
                }
            }
            catch (...)
            {
            }

            return params;
        };
    }
}
