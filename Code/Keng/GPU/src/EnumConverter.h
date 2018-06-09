#pragma once

#include "FwdDecl.h"
#include "d3d11.h"
#include "Keng/GraphicsCommon/DeviceBufferBindFlags.h"
#include "Keng/GraphicsCommon/DeviceBufferUsage.h"
#include "Keng/GraphicsCommon/FragmentFormat.h"
#include "Keng/GraphicsCommon/TextureAddressMode.h"
#include "Keng/GraphicsCommon/PrimitiveTopology.h"
#include "Keng/GraphicsCommon/CpuAccessFlags.h"

namespace keng::graphics::gpu
{
    DXGI_FORMAT ConvertTextureFormat(const FragmentFormat&);
    FragmentFormat ConvertTextureFormat(const DXGI_FORMAT&);

    D3D11_TEXTURE_ADDRESS_MODE ConvertAddressMode(const TextureAddressMode&);
    TextureAddressMode ConvertAddressMode(const D3D11_TEXTURE_ADDRESS_MODE&);

    D3D11_FILTER ConvertFilteringMode(const FilteringMode&);

    PrimitiveTopology ConvertTopology(const D3D_PRIMITIVE_TOPOLOGY&);
    D3D_PRIMITIVE_TOPOLOGY ConvertTopology(const PrimitiveTopology&);

    CpuAccessFlags ConvertCpuAccessFlags(UINT flags);
    UINT ConvertCpuAccessFlags(const CpuAccessFlags& flags);

    UINT ConvertDeviceBufferBindFlags(const DeviceBufferBindFlags& flags);
    DeviceBufferBindFlags ConvertDeviceBufferBindFlags(UINT flags);

    D3D11_USAGE ConvertDeviceBufferUsage(const DeviceBufferUsage& flags);
    DeviceBufferUsage ConvertDeviceBufferUsage(D3D11_USAGE flags);
}
