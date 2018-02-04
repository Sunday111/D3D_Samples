#pragma once

#include <istream>
#include <fstream>
#include <vector>
#include "ResourceSystem.h"
#include "EverydayTools\Array\ArrayView.h"

class FileResource : public RefCountImpl<IResource> {
public:
    void ReadFromFile(const char* filename) override {
        CallAndRethrow("FileResource::ReadFromFile", [&]() {
            std::ifstream file(filename, std::ios::binary | std::ios::ate);
            m_filename = filename;
            if (!file.is_open()) {
                std::string message("Could not open this file: ");
                message += filename;
                throw std::runtime_error(std::move(message));
            }
            auto size = file.tellg();
            file.seekg(0, std::ios::beg);
            m_buffer.resize(static_cast<size_t>(size) + 1);
            file.read((char*)&m_buffer[0], size);
        });
    }

    const char* GetFilename() const override {
        return m_filename.c_str();
    }

    int GetInvalidateSeconds() const override {
        return m_invalidateSeconds;
    }

    void SetInvalidateSeconds(int value) {
        m_invalidateSeconds = value;
    }

    edt::DenseArrayView<const uint8_t> GetDataView() const {
        return edt::DenseArrayView<const uint8_t>(m_buffer.data(), m_buffer.size());
    }

private:
    int m_invalidateSeconds = 0;
    std::string m_filename;
    std::vector<uint8_t> m_buffer;
};