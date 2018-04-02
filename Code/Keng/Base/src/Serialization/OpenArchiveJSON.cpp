#include "Keng/Base/Serialization/OpenArchiveJSON.h"

namespace keng
{
    void OpenArchiveJSON(const std::vector<uint8_t>& buffer, yasli::JSONIArchive& ar) {
        return CallAndRethrowM + [&] {
            auto openOk = ar.open((const char*)buffer.data(), buffer.size());
            edt::ThrowIfFailed(openOk, "Failed to open JSON archive");
        };
    }
}
