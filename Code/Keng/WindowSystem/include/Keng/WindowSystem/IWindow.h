#pragma once

#include <cstdint>

namespace keng::window_system
{
    class IWindowListener;

    class IWindow
    {
    public:
        virtual void Subscribe(IWindowListener*) = 0;
        virtual void Unsubscribe(IWindowListener*) = 0;
        virtual void GetClientSize(size_t* w, size_t* h) = 0;
        virtual int64_t* GetNativeHandle() = 0;
        virtual ~IWindow() = default;
    };
}
