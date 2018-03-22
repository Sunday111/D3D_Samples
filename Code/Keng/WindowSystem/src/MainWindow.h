#pragma once

#include "Keng/WindowSystem/IWindowListener.h"
#include "Keng/WindowSystem/IWindow.h"
#include "WinWrappers\WinWrappers.h"
#include "EverydayTools\UnusedVar.h"
#include "EverydayTools\Observable.h"

#include <algorithm>
#include <vector>

namespace keng::window_system
{
	template<typename TChar>
	class MainWindow :
        public IWindow,
		public Window<TChar, ::keng::window_system::MainWindow>,
		public Observable<MainWindow<TChar>, IWindowListener>
	{
	public:

		MainWindow(HWND handle = nullptr) :
			Window<TChar, keng::window_system::MainWindow>(handle) {
		}

		void Initialize(HINSTANCE hInstance) {
			UnusedVar(hInstance);
		}

		LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
			switch (msg)
			{
			case WM_SIZE:
				ForEachListener([&](IWindowListener* p) {
					p->OnWindowResize(LOWORD(lParam), HIWORD(lParam));
					return false;
				});
				break;
			}

			return WA::DefWindowProc_(hWnd, msg, wParam, lParam);
		}

        //IWindow
        virtual void Subscribe(IWindowListener* listener) override
        {
            Observable<window_system::MainWindow<TChar>, IWindowListener>::Subscribe(listener);
        }
        virtual void Unsubscribe(IWindowListener* listener) override
        {
            Observable<window_system::MainWindow<TChar>, IWindowListener>::Unsubscribe(listener);
        }
        virtual void GetClientSize(uint32_t* w, uint32_t* h) override
        {
            GetWindowClientSize(w, h);
        }
        virtual int64_t* GetNativeHandle() override {
            return (int64_t*)GetHandle();
        }
	};

	template<typename TChar>
	class MainWindowClass :
        public WindowClass<TChar, keng::window_system::MainWindowClass> {
	public:
		using Window = MainWindow<TChar>;
		static inline const TChar* GetName();

		MainWindowClass(HINSTANCE module_) :
			WindowClass<TChar, keng::window_system::MainWindowClass>(module_, 0)
		{

		}
	};

	const char* MainWindowClass<char>::GetName() { return "MainWindow"; }
	const wchar_t* MainWindowClass<wchar_t>::GetName() { return L"MainWindow"; }
}
