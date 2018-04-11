#pragma once
#include "EverydayTools/Delegate.h"
#include "EverydayTools/Exception/CallAndRethrow.h"
#include "EverydayTools/Exception/ThrowIfFailed.h"
#include "Keng/FileSystem/IFile.h"
#include "Keng/FileSystem/IFileSystem.h"
#include "Keng/FileSystem/OpenFileParameters.h"
#include <memory>
#include <string_view>

namespace keng::filesystem
{
    inline std::pair<std::unique_ptr<uint8_t>, size_t> ReadFileToBuffer(IFileSystem& system, std::string_view filename) {
        return CallAndRethrowM + [&] {
            OpenFileParameters openFileParameters{};
            openFileParameters.accessFlags |= FileAccessFlags::Read;
            auto file = system.GetFile(filename, openFileParameters);
            std::pair<std::unique_ptr<uint8_t>, size_t> result;
            auto size = file->GetSize();
            edt::ThrowIfFailed(size > 0, "File \"", filename, "\" is empty");
            result.first.reset(new uint8_t[size]);
            result.second = size;
            file->Read(0, size, edt::DenseArrayView<uint8_t>(result.first.get(), size));
            return result;
        };
    }

    inline void HandleFileData(IFileSystem& system, std::string_view filename, edt::Delegate<void(edt::DenseArrayView<const uint8_t>)> delegate) {
        CallAndRethrowM + [&] {
            size_t size;
            std::unique_ptr<uint8_t> buffer;

            std::tie(buffer, size) = ReadFileToBuffer(system, filename);
            delegate.Invoke(edt::DenseArrayView<const uint8_t>(buffer.get(), size));
        };
    }
}
