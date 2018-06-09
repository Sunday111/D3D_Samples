#pragma once

#include "FwdDecl.h"
#include "d3d11.h"
#include "Keng/GPU/FragmentFormat.h"
#include "Keng/GraphicsCommon/TextureAddressMode.h"
#include "Keng/GPU/FilteringMode.h"
#include "Keng/GPU/PrimitiveTopology.h"
#include "Keng/GPU/CpuAccessFlags.h"

namespace keng::graphics::gpu
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
