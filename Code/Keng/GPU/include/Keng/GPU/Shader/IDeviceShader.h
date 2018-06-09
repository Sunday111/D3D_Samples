#pragma once

#include "Keng/GPU/FwdDecl.h"
#include "Keng/GraphicsCommon/ShaderType.h"
#include "Keng/Core/IRefCountObject.h"

namespace keng::graphics::gpu
{
    class IDeviceShader : public core::IRefCountObject
    {
    public:
        virtual ShaderType GetShaderType() const = 0;
        virtual void AssignToPipeline() const = 0;
    };

    template<ShaderType type>
    class IShaderT : public IDeviceShader
    {

    };

    template<>
    class IShaderT<ShaderType::Vertex> : public IDeviceShader
    {
    public:
        virtual IInputLayoutPtr MakeDefaultInputLayout() = 0;
    };

    using IVertexShader = IShaderT<ShaderType::Vertex>;
    using IVertexShaderPtr = core::Ptr<IVertexShader>;

    using IFragmentShader = IShaderT<ShaderType::Fragment>;
    using IFragmentShaderPtr = core::Ptr<IFragmentShader>;
}
