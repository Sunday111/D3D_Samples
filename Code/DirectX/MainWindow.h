#pragma once

#include "WinWrappers\WinWrappers.h"
#include "EverydayTools\UnusedVar.h"

template<typename TChar>
class MainWindow :
    public Window<TChar, ::MainWindow> {
public:
    MainWindow(HWND handle = nullptr) :
        Window<TChar, ::MainWindow>(handle) {
    }

    void Initialize(HINSTANCE hInstance) {
        UnusedVar(hInstance);
    }

private:
};

template<typename TChar>
class MainWindowClass :
    public WindowClass<TChar, ::MainWindowClass> {
public:
    using Window = MainWindow<TChar>;
    static const TChar* GetName();

    MainWindowClass(HINSTANCE module_) :
        WindowClass<TChar, ::MainWindowClass>(module_, 0)
    {

    }
};

const char* MainWindowClass<char>::GetName() { return "MainWindow"; }
const wchar_t* MainWindowClass<wchar_t>::GetName() { return L"MainWindow"; }