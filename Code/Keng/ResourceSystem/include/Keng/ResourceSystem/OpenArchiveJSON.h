#pragma once

#include <fstream>
#include <string_view>
#include <vector>
#include "EverydayTools/Exception/CallAndRethrow.h"
#include "EverydayTools/Exception/ThrowIfFailed.h"
#include "yasli/JSONIArchive.h"

namespace keng::resource
{
    inline std::vector<uint8_t> ReadFileToBuffer(std::string_view filename) {
        return CallAndRethrowM + [&] {
            std::ifstream file(filename.data(), std::ios::binary | std::ios::ate);
            edt::ThrowIfFailed(file.is_open(), "Could not open file \"", filename, "\"");
            std::streamsize size = file.tellg();
            file.seekg(0, std::ios::beg);
            std::vector<uint8_t> buffer(size);
            file.read((char*)buffer.data(), size);
            return buffer;
        };
    }

    inline void OpenArchiveJSON(const std::vector<uint8_t>& buffer, yasli::JSONIArchive& ar) {
        return CallAndRethrowM + [&] {
            auto openOk = ar.open((const char*)buffer.data(), buffer.size());
            edt::ThrowIfFailed(openOk, "Failed to open JSON archive");
        };
    }

    template<typename T>
    inline void SerializeMandatory(yasli::Archive& ar, T& val, const char* name = "") {
        CallAndRethrowM + [&] {
            edt::ThrowIfFailed(ar(val, name), "Failed to parse mandatory field \"", name, "\"");
        };
    }
}