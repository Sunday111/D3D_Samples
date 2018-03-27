#include "d3d_tools/Texture.h"

namespace keng::window_system
{
    class IWindow;
}

namespace keng::graphics
{
    class WindowRenderTargetParameters
    {
    public:
        d3d_tools::TextureFormat format = d3d_tools::TextureFormat::R8_G8_B8_A8_UNORM;
        window_system::IWindow* window;
    };
}