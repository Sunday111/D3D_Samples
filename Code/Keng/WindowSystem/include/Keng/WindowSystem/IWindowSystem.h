#pragma once

#include "Keng/Core/ISystem.h"
#include "Keng/WindowSystem/FwdDecl.h"

namespace keng::window_system
{
    class IWindowSystem : public core::ISystem
    {
    public:
        static std::string_view SystemName() { return "KengWindowSystem"; }
        virtual IWindow* GetWindow() = 0;
    };
}
