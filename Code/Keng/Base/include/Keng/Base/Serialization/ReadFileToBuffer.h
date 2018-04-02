#pragma once

#include <cstdint>
#include <vector>
#include <string_view>

namespace keng
{
    std::vector<uint8_t> ReadFileToBuffer(std::string_view filename);
}
