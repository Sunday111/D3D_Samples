#pragma once

#include <vector>
#include "EverydayTools/Exception/CallAndRethrow.h"
#include "EverydayTools/Exception/ThrowIfFailed.h"
#include "yasli/JSONIArchive.h"

namespace keng
{
    void OpenArchiveJSON(const std::vector<uint8_t>& buffer, yasli::JSONIArchive& ar);
}
