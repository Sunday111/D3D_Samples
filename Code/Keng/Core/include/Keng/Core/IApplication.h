#pragma once

#include "Keng/Core/FwdDecl.h"

#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace keng::core
{
    struct ApplicationStartupParameters
    {
        std::vector<std::string> modulesToLoad;
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
