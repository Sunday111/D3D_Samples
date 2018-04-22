#pragma once

#include "Keng/WindowSystem/IWindowSystem.h"
#include <string>
#include "MainWindow.h"
#include "WinWrappers/WinWrappers.h"
#include "Keng/Core/ISystem.h"
#include "Keng/Base/Serialization/Serialization.h"
#include "Keng/FileSystem/IFileSystem.h"

namespace yasli
{
    class Archive;
}

namespace keng::window_system
{
    class WindowSystem : public core::RefCountImpl<window_system::IWindowSystem>
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

        // IWindowSystem
        virtual IWindow* GetWindow() override;

        // ISystem
        virtual const char* GetSystemName() const override;
        virtual bool ForEachDependency(const edt::Delegate<bool(const char*)>& delegate) const override;
        virtual void Initialize(const core::IApplicationPtr& app) override;
        virtual bool Update() override;
        virtual void Shutdown() override;

    protected:
        std::vector<std::string> m_dependencies;

    private:
        std::unique_ptr<MainWindowClass<TChar>> m_windowClass;
        std::unique_ptr<MainWindow<TChar>> m_window;
    };
}
