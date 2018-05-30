#pragma once

#include "EverydayTools/Array/ArrayView.h"

namespace keng::gpu
{
    class ShaderDefinition
    {
    public:
        const char* name = nullptr;
        const char* value = nullptr;
    };

    class ShaderParameters
    {
    public:
        const char* code = nullptr;
        const char* entry_point = "main";
        edt::SparseArrayView<ShaderDefinition> definitions;
    };
}
