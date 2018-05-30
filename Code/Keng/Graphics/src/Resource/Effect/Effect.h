#pragma once

#include "FwdDecl.h"
#include "Keng/Graphics/Resource/IEffect.h"
#include "Resource/Shader/Shader.h"
#include "Keng/GPU/PipelineInput/IInputLayout.h"
#include "Keng/GPU/Shader/IShader.h"

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

        gpu::IInputLayoutPtr m_inputLayout;
        core::Ptr<Shader<gpu::ShaderType::Vertex>> vs;
        core::Ptr<Shader<gpu::ShaderType::Fragment>> fs;
    };

}
