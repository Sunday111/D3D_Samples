#pragma once

#include "FwdDecl.h"
#include "Keng/Graphics/Resource/IEffect.h"
#include "Resource/Shader/Shader.h"
#include "Keng/GPU/PipelineInput/IInputLayout.h"
#include "Keng/GPU/Shader/IDeviceShader.h"

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
        virtual void InitDefaultInputLayout() override final;
        virtual void AssignToPipeline() override final;

        gpu::IInputLayoutPtr m_inputLayout;
        core::Ptr<Shader<ShaderType::Vertex>> vs;
        core::Ptr<Shader<ShaderType::Fragment>> fs;
    };

}
