#pragma once

#include <string>
#include "MainWindow.h"
#include "WinWrappers/WinWrappers.h"
#include "Keng/Core/Systems/ISystem.h"

namespace keng
{
	class WindowSystem :
        public ISystem
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

		static const char* GetGUID();
        MainWindow<TChar>* GetWindow() const;
        static SystemParams ReadDefaultParams();

        // ISystem
        virtual bool ForEachSystemDependency(bool(*pfn)(const char* systemGUID, void* context), void* context) override;
        virtual const char* GetSystemGUID() override;
        virtual void Initialize(IApplication* app) override;
        virtual bool Update() override;

    protected:
        std::vector<std::string> m_dependencies;
	
	private:
	    std::unique_ptr<MainWindowClass<TChar>> m_windowClass;
	    std::unique_ptr<MainWindow<TChar>> m_window;
	};
}
