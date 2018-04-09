#include "File.h"
#include "EverydayTools/Exception/CallAndRethrow.h"
#include "EverydayTools/Exception/ThrowIfFailed.h"
#include <algorithm>
#include <fstream>

namespace keng::filesystem
{
    File::File(std::string_view name, const OpenFileParameters& parameters) {
        CallAndRethrowM + [&]() {
            m_parameters = parameters;
            m_name = name;
            edt::ThrowIfFailed((parameters.accessFlags & FileAccessFlags::Write) == FileAccessFlags::None, 
                "Reading files is not implemented yet");

            std::ifstream file(name.data(), std::ios::binary | std::ios::ate);
            edt::ThrowIfFailed(file.is_open(), "Could not open file \"", name, "\"");
            std::streamsize size = file.tellg();
            file.seekg(0, std::ios::beg);
            std::vector<uint8_t> m_data(size);
            file.read((char*)m_data.data(), size);
            return m_data;
        };
    }

    File::~File() {

    }

    size_t File::TryRead(size_t offset, size_t bytes, edt::DenseArrayView<uint8_t> dest) {
        return CallAndRethrowM + [&]() -> size_t {
            if (offset > m_data.size()) {
                return 0;
            }

            auto begin = m_data.begin();
            auto end = begin + std::min(m_data.size(), offset + std::min(bytes, dest.GetSize()));
            std::copy(begin, end, dest.begin());
            return static_cast<size_t>(std::distance(begin, end));
        };
    }

    const OpenFileParameters& File::GetParameters() const {
        return m_parameters;
    }

    size_t File::GetSize() const {
        return m_data.size();
    }
}
