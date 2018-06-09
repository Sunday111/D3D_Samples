#pragma once

#include "Keng/GPU/FwdDecl.h"
#include "Keng/GPU/FragmentFormat.h"
#include "Keng/GPU/Shader/ShaderType.h"
#include "EverydayTools/Array/ArrayView.h"

namespace keng::graphics::gpu
{
    class ITexture : public core::IRefCountObject
    {
    public:
        virtual FragmentFormat GetFormat() const = 0;
        virtual void AssignToPipeline(ShaderType shaderType, size_t slot) = 0;
        virtual void CopyTo(ITexturePtr) = 0;
        virtual void SetData(edt::DenseArrayView<const uint8_t> data) = 0;
        virtual size_t GetWidth() const = 0;
        virtual size_t GetHeight() const = 0;
        virtual IDevicePtr GetDevice() const = 0;
        virtual ~ITexture() = default;
    };
}
