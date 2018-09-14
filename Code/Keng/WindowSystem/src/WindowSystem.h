#pragma once

#include <string>
#include "Keng/Base/Serialization/Serialization.h"
#include "Keng/Core/System.h"
#include "Keng/FileSystem/IFileSystem.h"
#include "Keng/WindowSystem/IWindowSystem.h"
#include "MainWindow.h"
#include "WinWrappers/WinWrappers.h"

namespace yasli
{
    class Archive;
}

namespace keng::window_system
{
    class WindowSystem : public core::System
    <
        IWindowSystem, WindowSystem,
        filesystem::IFileSystem
    >
    {
    public:
        using TChar = char;
        using WA = WinAPI<TChar>;

        struct SystemParams
        {
            void serialize(Archive& ar);
            HINSTANCE hInstance = nullptr;
            std::string WindowTitle;
            int nCmdShow = SW_SHOW;
            unsigned Width = 1280;
            unsigned Height = 720;
        };

    public:
        WindowSystem();
        ~WindowSystem();

        MainWindow<TChar>* GetWindow() const;

        // ISystem
        virtual void OnSystemEvent(const keng::core::IApplicationPtr&, const  keng::core::SystemEvent&) override final;

        // IWindowSystem
        virtual IWindow* GetWindow() override final;

    private:
        // ISystem
        void Initialize(const core::IApplicationPtr& app);
        void Update(const core::IApplicationPtr& app);

    private:
        std::unique_ptr<MainWindowClass<TChar>> m_windowClass;
        std::unique_ptr<MainWindow<TChar>> m_window;
    };
}
