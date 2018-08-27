#pragma once

#include "ISystem.h"
#include "DependencyStorage.h"

namespace keng::core
{
    template<typename Final, typename... Dependencies>
    class System :
        public keng::core::RefCountImpl<keng::core::ISystem>,
        public DependenciesContainer<Dependencies...>
    {
    public:
        virtual const char* GetSystemName() const override {
            return Final::SystemName();
        }

        virtual bool ForEachDependency(const edt::Delegate<bool(const char*)>& delegate) const override {
            return CallAndRethrowM + [&]() -> bool {
                return (delegate.Invoke(Dependencies::SystemName()) || ... || false);
            };
        }
    };
}
