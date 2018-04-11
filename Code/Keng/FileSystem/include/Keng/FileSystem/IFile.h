#pragma once

#include "EverydayTools/Array/ArrayView.h"
#include "Keng/FileSystem/FwdDecl.h"

namespace keng::filesystem
{
    class IFile : public core::IRefCountObject
    {
    public:
        virtual size_t TryRead(size_t offset, size_t bytes, edt::DenseArrayView<uint8_t> destination) = 0;
        virtual const OpenFileParameters& GetParameters() const = 0;
        virtual size_t GetSize() const = 0;
    };
}
