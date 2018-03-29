#include "Keng/Graphics/FragmentFormat.h"

namespace keng::window_system
{
    class IWindow;
}

namespace keng::graphics
{
    class WindowRenderTargetParameters
    {
    public:
        FragmentFormat format = FragmentFormat::R8_G8_B8_A8_UNORM;
        window_system::IWindow* window;
    };
}