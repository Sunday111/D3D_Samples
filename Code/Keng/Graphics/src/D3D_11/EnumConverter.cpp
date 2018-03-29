#include "EnumConverter.h"
#include "EverydayTools/Exception/CallAndRethrow.h"

namespace keng::graphics::d3d
{
    DXGI_FORMAT ConvertTextureFormat(FragmentFormat from) {
        return CallAndRethrowM + [&] {
            switch (from) {
            case FragmentFormat::R8_G8_B8_A8_UNORM: return DXGI_FORMAT_R8G8B8A8_UNORM;
            case FragmentFormat::R24_G8_TYPELESS: return DXGI_FORMAT_R24G8_TYPELESS;
            case FragmentFormat::D24_UNORM_S8_UINT: return DXGI_FORMAT_D24_UNORM_S8_UINT;
            case FragmentFormat::R24_UNORM_X8_TYPELESS: return DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
            default: throw std::runtime_error("This texture format is not implemented here");
            }
        };
    }

    FragmentFormat ConvertTextureFormat(DXGI_FORMAT from) {
        return CallAndRethrowM + [&] {
            switch (from) {
            case DXGI_FORMAT_R8G8B8A8_UNORM: return FragmentFormat::R8_G8_B8_A8_UNORM;
            case DXGI_FORMAT_R24G8_TYPELESS: return FragmentFormat::R24_G8_TYPELESS;
            case DXGI_FORMAT_D24_UNORM_S8_UINT: return FragmentFormat::D24_UNORM_S8_UINT;
            case DXGI_FORMAT_R24_UNORM_X8_TYPELESS: return FragmentFormat::R24_UNORM_X8_TYPELESS;
            default: throw std::runtime_error("This texture format is not implemented here");
            }
        };
    }
}
