#include "Resource/Effect/Effect.h"
#include "Keng/Graphics/GraphicsSystem.h"
#include "EverydayTools/Array/ArrayView.h"
#include "EverydayTools/Array/ArrayViewVector.h"
#include "EverydayTools/Exception/CallAndRethrow.h"
#include "EverydayTools/Exception/ThrowIfFailed.h"

namespace keng::graphics
{
    void Effect::Use(IDevice* device) {
        CallAndRethrowM + [&] {
            assert(dynamic_cast<Device*>(device));
            auto casted = static_cast<Device*>(device);
            casted->SetShader(vs->m_impl);
            casted->SetShader(fs->m_impl);
            casted->GetContext()->IASetInputLayout(layout.Get());
        };
    }

    void Effect::InitDefaultInputLayout(IDevice* device) {
        CallAndRethrowM + [&] {
            std::vector<D3D11_INPUT_ELEMENT_DESC> layouts;
            edt::ThrowIfFailed(vs != nullptr, "vertex shader is not initialized");
            vs->m_impl.ReflectInputLayout(layouts);
            assert(dynamic_cast<Device*>(device));
            layout = static_cast<Device*>(device)->CreateInputLayout(edt::MakeArrayView(layouts), vs->m_impl.bytecode.Get());
        };
    }
}
