#pragma once

#include "EverydayTools/Array/ArrayView.h"
#include "yasli/JSONIArchive.h"

namespace keng
{
    void OpenArchiveJSON(edt::DenseArrayView<const uint8_t> buffer, yasli::JSONIArchive& ar);
}
