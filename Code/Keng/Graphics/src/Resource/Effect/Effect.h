#pragma once

#include "FwdDecl.h"
#include "Keng/Graphics/Resource/IEffect.h"
#include "Resource/Shader/Shader.h"

namespace keng::graphics
{
    class Device;

    class Effect :
        public core::RefCountImpl<IEffect>
    {
    public:
        Effect();
        ~Effect();

        // IEffect
        virtual void InitDefaultInputLayout() override;
        virtual void AssignToPipeline() override;

        DevicePtr device;
        ComPtr<ID3D11InputLayout> layout;
        core::Ptr<Shader<d3d_tools::ShaderType::Vertex>> vs;
        core::Ptr<Shader<d3d_tools::ShaderType::Pixel>> fs;
    };

}
