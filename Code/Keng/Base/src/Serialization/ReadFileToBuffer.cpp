#include "Keng/Base/Serialization/ReadFileToBuffer.h"
#include "EverydayTools/Exception/CallAndRethrow.h"
#include "EverydayTools/Exception/ThrowIfFailed.h"
#include <fstream>

namespace keng
{
    std::vector<uint8_t> ReadFileToBuffer(std::string_view filename) {
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
}
