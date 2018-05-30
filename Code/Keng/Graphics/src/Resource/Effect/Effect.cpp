#include "Resource/Effect/Effect.h"
#include "Resource/Shader/Shader.h"
#include "EverydayTools/Array/ArrayView.h"
#include "EverydayTools/Array/ArrayViewVector.h"
#include "EverydayTools/Exception/CallAndRethrow.h"
#include "EverydayTools/Exception/ThrowIfFailed.h"
#include "Device.h"

namespace keng::graphics
{
    Effect::Effect() = default;

    Effect::~Effect() = default;

    void Effect::AssignToPipeline() {
        CallAndRethrowM + [&] {
            vs->m_impl->AssignToPipeline();
            fs->m_impl->AssignToPipeline();
            m_inputLayout->AssignToPipeline();
        };
    }

    void Effect::InitDefaultInputLayout() {
        CallAndRethrowM + [&] {
            m_inputLayout = vs->m_impl->MakeDefaultInputLayout();
        };
    }
}
