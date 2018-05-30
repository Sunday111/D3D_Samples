#pragma once

#include "FwdDecl.h"
#include "Keng/Graphics/Resource/IEffect.h"
#include "Resource/Shader/Shader.h"
#include "Keng/GraphicsAPI/IInputLayout.h"
#include "Keng/GraphicsAPI/Shader/IShader.h"

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

        graphics_api::IInputLayoutPtr m_inputLayout;
        core::Ptr<Shader<graphics_api::ShaderType::Vertex>> vs;
        core::Ptr<Shader<graphics_api::ShaderType::Fragment>> fs;
    };

}
