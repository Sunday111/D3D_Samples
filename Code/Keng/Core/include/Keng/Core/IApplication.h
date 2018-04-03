#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace keng::core
{
    class ISystem;

    struct ApplicationStartupParameters
    {
        std::vector<std::string> modulesToLoad;
    };

    class IApplication
    {
    public:
        virtual bool Update() = 0;
        virtual void Run() = 0;
        virtual void Initialize(const ApplicationStartupParameters& params) = 0;
        virtual ISystem* FindSystem(std::string_view name) const = 0;

        template<typename T, typename Enable = std::enable_if_t<std::is_base_of_v<ISystem, T>>>
        T* FindSystem() const {
            auto abstract = FindSystem(T::SystemName());
            return dynamic_cast<T*>(abstract);
        }

        virtual ~IApplication() = default;
    };
}
