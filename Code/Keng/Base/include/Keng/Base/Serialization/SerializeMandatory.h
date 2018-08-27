#pragma once

#include "EverydayTools/Exception/CallAndRethrow.h"
#include "EverydayTools/Exception/ThrowIfFailed.h"
#include "Keng/Base/Serialization/Serialization.h"
#include "yasli/Archive.h"

namespace keng
{
    template<typename T>
    inline void SerializeMandatory(Archive& ar, T& val, const char* name = "") {
        CallAndRethrowM + [&] {
            edt::ThrowIfFailed(ar(val, name), "Failed to parse mandatory field \"", name, "\"");
        };
    }
}
