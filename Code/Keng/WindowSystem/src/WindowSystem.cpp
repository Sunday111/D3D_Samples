#include "WindowSystem.h"
#include "EverydayTools/Array/ArrayViewVector.h"
#include "Keng/Core/IApplication.h"
#include "Keng/Base/Serialization/SerializeMandatory.h"
#include "keng/Base/Serialization/OpenArchiveJSON.h"
#include "Keng/FileSystem/ReadFileToBuffer.h"
#include "yasli/JSONIArchive.h"
#include "yasli/STL.h"

namespace keng::window_system
{
    namespace
    {
        static WindowSystem::SystemParams ReadDefaultParams() {
            return CallAndRethrowM + [&] {
                struct File
                {
                    void serialize(Archive& ar) {
                        SerializeMandatory(ar, params, "window_system");
                    }
                    WindowSystem::SystemParams params;
                };

                File file{};
                file.params.hInstance = GetModuleHandle(NULL);
                file.params.WindowTitle = "MainWindow";

                try {

                    auto filename = "Configs/window_system.json";
                    using FileView = edt::DenseArrayView<const uint8_t>;
                    auto onFileRead = [&](FileView fileView) {
                        yasli::JSONIArchive ar;
                        OpenArchiveJSON(fileView, ar);
                        ar(file);
                    };
                    edt::Delegate<void(FileView)> delegate;
                    delegate.Bind(onFileRead);
                    filesystem::HandleFileData(filename, delegate);
                } catch (...) {
                }

                return file.params;
            };
        }
    }

    WindowSystem::WindowSystem() = default;

    WindowSystem::~WindowSystem() = default;

    void WindowSystem::Initialize(const core::IApplicationPtr& app) {
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

    std::string_view WindowSystem::GetSystemName() const {
        return SystemName();
    }

    bool WindowSystem::ForEachDependency(const edt::Delegate<bool(std::string_view)>& delegate) const {
        return CallAndRethrowM + [&]() -> bool {
            std::string_view dependencies[] =
            {
                filesystem::IFileSystem::SystemName()
            };

            for (auto& systemName : dependencies) {
                if (delegate.Invoke(systemName)) {
                    return true;
                }
            }

            return false;
        };
    }

    IWindow* WindowSystem::GetWindow() {
        return m_window.get();
    }
}
