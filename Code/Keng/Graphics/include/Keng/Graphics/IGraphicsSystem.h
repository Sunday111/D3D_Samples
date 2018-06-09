#pragma once

#include "Keng/Graphics/FwdDecl.h"
#include "Keng/Core/ISystem.h"
#include "Keng/GraphicsCommon/PrimitiveTopology.h"
#include "EverydayTools/Array/ArrayView.h"

namespace keng::graphics
{
    class IGraphicsSystem : public core::ISystem
    {
    public:
        static const char* SystemName() { return "KengGraphics"; }
        virtual IDevicePtr GetDevice() = 0;
        virtual ITexturePtr CreateTexture(const gpu::TextureParameters& params) = 0;
    };
}
