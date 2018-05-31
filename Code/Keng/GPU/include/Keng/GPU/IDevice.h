#pragma once

#include "Keng/Core/IRefCountObject.h"
#include "Keng/GPU/FwdDecl.h"
#include "Keng/GPU/Shader/ShaderType.h"
#include "Keng/GPU/PrimitiveTopology.h"
#include "EverydayTools/Array/ArrayView.h"

namespace keng::gpu
{
    class IDevice : public core::IRefCountObject
    {
    public:
        virtual ITexturePtr CreateTexture(const TextureParameters& params) = 0;
        virtual IShaderPtr CreateShader(ShaderType type, const ShaderParameters& parameters) = 0;
        virtual IDeviceBufferPtr CreateDeviceBuffer(const DeviceBufferParameters& params, edt::DenseArrayView<const uint8_t> data) = 0;
        virtual void SetTopology(PrimitiveTopology topo) = 0;
        virtual void* GetNativeDevice() const = 0;
        virtual void* GetNativeContext() const = 0;
        virtual ~IDevice() = default;
    };
}
