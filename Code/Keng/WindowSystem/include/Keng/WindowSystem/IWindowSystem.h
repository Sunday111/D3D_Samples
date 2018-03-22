#pragma once

#include "Keng/Core/ISystem.h"

namespace keng::window_system
{
    class IWindow;

    class IWindowSystem :
        public core::ISystem
    {
    public:
        static const char* GetGUID() { return "4CD74D12-0159-44EC-B79D-9D1422221A5C"; }
        virtual IWindow* GetWindow() = 0;
    };
}
