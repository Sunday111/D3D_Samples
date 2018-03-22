#pragma once

namespace keng::window_system
{
    class IWindowListener {
    public:
        virtual void OnWindowResize(int w, int h) = 0;
        virtual ~IWindowListener() = default;
    };
}
