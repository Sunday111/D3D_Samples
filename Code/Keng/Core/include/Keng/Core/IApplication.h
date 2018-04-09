#pragma once

#include "Keng/Core/FwdDecl.h"
#include "EverydayTools/Array/ArrayView.h"

#include <memory>
#include <string>
#include <string_view>

namespace keng::core
{
    class ApplicationStartupParameters
    {
    public:
        edt::SparseArrayView<const std::string> modulesToLoad;
    };

    class IApplication : public IRefCountObject
    {
    public:
        virtual bool Update() = 0;
        virtual void Run() = 0;
        virtual void Initialize(const ApplicationStartupParameters& params) = 0;
        virtual ISystemPtr FindSystem(std::string_view name) const = 0;

        template<typename T, typename Enable = std::enable_if_t<std::is_base_of_v<ISystem, T>>>
        core::Ptr<T> FindSystem() const {
            return std::dynamic_pointer_cast<T>(FindSystem(T::SystemName()));
        }

        virtual ~IApplication() = default;
    };
}
