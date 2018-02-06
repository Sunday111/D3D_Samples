#pragma once

#include "WinWrappers\WinWrappers.h"
#include "EverydayTools\UnusedVar.h"
#include "EverydayTools\Observable.h"

#include <algorithm>
#include <vector>

class IMainWindowListener {
public:
    virtual void OnWindowResize(int w, int h) = 0;
    virtual ~IMainWindowListener() = default;
};

template<typename TChar>
class MainWindow :
    public Window<TChar, ::MainWindow>,
    public Observable<MainWindow<TChar>, IMainWindowListener>
{
public:

    MainWindow(HWND handle = nullptr) :
        Window<TChar, ::MainWindow>(handle) {
    }

    void Initialize(HINSTANCE hInstance) {
        UnusedVar(hInstance);
    }

    LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        switch (msg)
        {
        case WM_SIZE:
            ForEachListener([&](IMainWindowListener* p) {
                p->OnWindowResize(LOWORD(lParam), HIWORD(lParam));
                return false;
            });
            break;
        }

        return WA::DefWindowProc_(hWnd, msg, wParam, lParam);
    }
};

template<typename TChar>
class MainWindowClass :
    public WindowClass<TChar, ::MainWindowClass> {
public:
    using Window = MainWindow<TChar>;
    static inline const TChar* GetName();

    MainWindowClass(HINSTANCE module_) :
        WindowClass<TChar, ::MainWindowClass>(module_, 0)
    {

    }
};

const char* MainWindowClass<char>::GetName() { return "MainWindow"; }
const wchar_t* MainWindowClass<wchar_t>::GetName() { return L"MainWindow"; }