#include "Resource/Effect/Effect.h"
#include "Keng/Graphics/GraphicsSystem.h"
#include "Resource/Shader/Shader.h"
#include "EverydayTools/Array/ArrayView.h"
#include "EverydayTools/Array/ArrayViewVector.h"
#include "EverydayTools/Exception/CallAndRethrow.h"
#include "EverydayTools/Exception/ThrowIfFailed.h"
#include "Keng/Graphics/Device.h"

namespace keng::graphics
{
    Effect::Effect() = default;

    Effect::~Effect() = default;

    void Effect::AssignToPipeline() {
        CallAndRethrowM + [&] {
            device->SetShader(vs->m_impl);
            device->SetShader(fs->m_impl);
            device->GetContext()->IASetInputLayout(layout.Get());
        };
    }

    void Effect::InitDefaultInputLayout() {
        CallAndRethrowM + [&] {
            std::vector<D3D11_INPUT_ELEMENT_DESC> layouts;
            edt::ThrowIfFailed(vs != nullptr, "vertex shader is not initialized");
            vs->m_impl.ReflectInputLayout(layouts);
            layout = device->CreateInputLayout(edt::MakeArrayView(layouts), vs->m_impl.bytecode.Get());
        };
    }
}
