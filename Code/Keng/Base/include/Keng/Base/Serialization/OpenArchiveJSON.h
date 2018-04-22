#pragma once

#include "EverydayTools/Array/ArrayView.h"
#include "EverydayTools/Exception/CallAndRethrow.h"
#include "EverydayTools/Exception/ThrowIfFailed.h"
#include "yasli/JSONIArchive.h"

namespace keng
{
    inline void OpenArchiveJSON(edt::DenseArrayView<const uint8_t> buffer, yasli::JSONIArchive& ar) {
        return CallAndRethrowM + [&] {
            auto openOk = ar.open((const char*)buffer.GetData(), buffer.GetSize());
            edt::ThrowIfFailed(openOk, "Failed to open JSON archive");
        };
    }
}
