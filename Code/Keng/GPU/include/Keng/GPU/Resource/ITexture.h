#pragma once

#include "Keng/GPU/FwdDecl.h"
#include "Keng/GraphicsCommon/FragmentFormat.h"
#include "EverydayTools/Array/ArrayView.h"

namespace keng::graphics::gpu
{
    class ITexture : public core::IRefCountObject
    {
    public:
        virtual FragmentFormat GetFormat() const = 0;
        virtual void AssignToPipeline(const ShaderType& shaderType, size_t slot) = 0;
        virtual void CopyTo(ITexture&) const = 0;
        virtual void SetData(edt::DenseArrayView<const uint8_t> data) = 0;
        virtual size_t GetWidth() const = 0;
        virtual size_t GetHeight() const = 0;
        virtual IDevicePtr GetDevice() const = 0;
        virtual ~ITexture() = default;
    };
}
