#pragma once

#include "WinWrappers\WinWrappers.h"
#include "EverydayTools\UnusedVar.h"

#include <algorithm>
#include <vector>

template<typename TChar>
class MainWindow :
    public Window<TChar, ::MainWindow> {
public:
    class IObserver {
    public:
        virtual void OnWindowResize(int w, int h) = 0;
        virtual ~IObserver() = default;
    };

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
            ForEachObserver([&](IObserver* p) {
                p->OnWindowResize(LOWORD(lParam), HIWORD(lParam));
                return false;
            });
            break;
        }

        return WA::DefWindowProc_(hWnd, msg, wParam, lParam);
    }

    template<typename F>
    bool ForEachObserver(F&& f) {
        for (auto& observer : m_observers) {
            if (f(observer)) {
                return true;
            }
        }
        return false;
    }

    void Subscribe(IObserver* observer) {
        auto it = std::lower_bound(m_observers.begin(), m_observers.end(), observer);
        if (it != m_observers.end() && *it == observer) {
            throw std::runtime_error("Already subscribed!");
        }
        m_observers.insert(it, observer);
    }

    void Unsubscribe(IObserver* observer) {
        auto it = std::lower_bound(m_observers.begin(), m_observers.end(), observer);
        if (it == m_observers.end() || *it != observer) {
            throw std::runtime_error("Failed to unsubscribe: observer not found!");
        }
        m_observers.erase(observer);
    }

private:
    std::vector<IObserver*> m_observers;
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