#pragma once

#include "Keng/Core/IRefCountObject.h"
#include "Keng/GraphicsAPI/FwdDecl.h"
#include "Keng/GraphicsAPI/Shader/ShaderType.h"

namespace keng::graphics_api
{
    class IDevice : public core::IRefCountObject
    {
    public:
        virtual ITexturePtr CreateTexture(const TextureParameters& params) = 0;
        virtual IShaderPtr CreateShader(ShaderType type, const ShaderParameters& parameters) = 0;
        virtual void* GetNativeDevice() const = 0;
        virtual void* GetNativeContext() const = 0;
        virtual ~IDevice() = default;
    };
}
