#pragma once

#include "FwdDecl.h"
#include "d3d11.h"
#include "Keng/Graphics/FragmentFormat.h"
#include "Keng/Graphics/TextureAddressMode.h"
#include "Keng/Graphics/FilteringMode.h"
#include "Keng/Graphics/PrimitiveTopology.h"
#include "Keng/Graphics/CpuAccessFlags.h"

namespace keng::graphics::d3d
{
    DXGI_FORMAT ConvertTextureFormat(FragmentFormat);
    FragmentFormat ConvertTextureFormat(DXGI_FORMAT);

    D3D11_TEXTURE_ADDRESS_MODE ConvertAddressMode(TextureAddressMode);
    TextureAddressMode ConvertAddressMode(D3D11_TEXTURE_ADDRESS_MODE);
    D3D11_FILTER ConvertFilteringMode(FilteringMode);

    PrimitiveTopology ConvertTopology(D3D_PRIMITIVE_TOPOLOGY);
    D3D_PRIMITIVE_TOPOLOGY ConvertTopology(PrimitiveTopology);

    CpuAccessFlags ConvertCpuAccessFlags(UINT flags);
    UINT ConvertCpuAccessFlags(CpuAccessFlags flags);
}
