#pragma once

#include "d3d11.h"
#include "Keng/Graphics/FragmentFormat.h"

namespace keng::graphics::d3d
{
    DXGI_FORMAT ConvertTextureFormat(FragmentFormat);
    FragmentFormat ConvertTextureFormat(DXGI_FORMAT);
}
