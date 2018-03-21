#pragma once

#include <string>
#include "MainWindow.h"
#include "WinWrappers/WinWrappers.h"
#include "Keng/Core/Systems/System.h"

namespace keng
{
	class WindowSystem :
		public System<WindowSystem> {
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

		static const char* GetGUID();
        virtual void Initialize(IApplication* app) override;
        bool Update() override;
        MainWindow<TChar>* GetWindow() const;
        static SystemParams ReadDefaultParams();
	
	private:
	    std::unique_ptr<MainWindowClass<TChar>> m_windowClass;
	    std::unique_ptr<MainWindow<TChar>> m_window;
	};
}
