#pragma once

#include "EverydayTools/Array/ArrayView.h"

namespace keng::graphics
{
    class DeviceShaderDefinition
    {
    public:
        const char* name = nullptr;
        const char* value = nullptr;
    };

    class DeviceShaderParameters
    {
    public:
        const char* code = nullptr;
        const char* entry_point = "main";
        edt::SparseArrayView<DeviceShaderDefinition> definitions;
    };
}
