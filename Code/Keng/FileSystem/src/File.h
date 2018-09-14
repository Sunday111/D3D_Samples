#include "FwdDecl.h"
#include "Keng/FileSystem/IFile.h"
#include "Keng/FileSystem/OpenFileParameters.h"
#include <vector>
#include <string_view>

namespace keng::filesystem
{
    class File : public core::RefCountImpl<IFile>
    {
    public:
        File(std::string_view name, const OpenFileParameters& parameters);
        ~File();

        // IFile
        virtual size_t TryRead(size_t offset, size_t bytes, edt::DenseArrayView<uint8_t> destination) override final;
        virtual void Read(size_t offset, size_t bytes, edt::DenseArrayView<uint8_t> destination) override final;
        virtual const OpenFileParameters& GetParameters() const override final;
        virtual size_t GetSize() const override final;

    private:
        std::string m_name;
        OpenFileParameters m_parameters;
        std::vector<uint8_t> m_data;
    };
}
