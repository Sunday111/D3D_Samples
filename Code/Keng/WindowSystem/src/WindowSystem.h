#pragma once

#include "Keng/WindowSystem/IWindowSystem.h"
#include <string>
#include "MainWindow.h"
#include "WinWrappers/WinWrappers.h"
#include "Keng/Core/ISystem.h"

namespace keng::window_system
{
	class WindowSystem :
        public window_system::IWindowSystem
    {
	public:
	    using TChar = char;
	    using WA = WinAPI<TChar>;
	
	    struct SystemParams {
	        HINSTANCE hInstance = nullptr;
	        std::string WindowTitle;
	        int nCmdShow = SW_SHOW;
	        unsigned Width = 1280;
	        unsigned Height = 720;
	    };

        MainWindow<TChar>* GetWindow() const;
        static SystemParams ReadDefaultParams();

        // IWindowSystem
        virtual IWindow* GetWindow() override;

        // ISystem
        virtual bool ForEachSystemDependency(bool(*pfn)(const char* systemGUID, void* context), void* context) override;
        virtual const char* GetSystemGUID() override;
        virtual void Initialize(core::IApplication* app) override;
        virtual bool Update() override;

    protected:
        std::vector<std::string> m_dependencies;
	
	private:
	    std::unique_ptr<MainWindowClass<TChar>> m_windowClass;
	    std::unique_ptr<MainWindow<TChar>> m_window;
	};

    __declspec(dllexport) WindowSystem* CreateWindowSystem();
}
