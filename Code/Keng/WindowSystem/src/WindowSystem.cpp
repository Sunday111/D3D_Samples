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
        static WindowSystem::SystemParams ReadDefaultParams(core::IApplication& app) {
            return CallAndRethrowM + [&] {
                auto fileSystem = app.FindSystem<filesystem::IFileSystem>();
                edt::ThrowIfFailed(fileSystem != nullptr, "IFileSystem is nullptr!");

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
                    filesystem::HandleFileData(*fileSystem, filename, delegate);
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
            auto params = ReadDefaultParams(*app);
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

    void WindowSystem::Shutdown() {

    }

    MainWindow<WindowSystem::TChar>* WindowSystem::GetWindow() const {
        return m_window.get();
    }

    void WindowSystem::SystemParams::serialize(Archive& ar) {
        ar(Width, "width");
        ar(Height, "height");
        ar(WindowTitle, "title");
    }

    IWindow* WindowSystem::GetWindow() {
        return m_window.get();
    }
}
