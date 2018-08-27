#pragma once

#include "DependencyStorage.h"
#include "EverydayTools/UnusedVar.h"

namespace keng::core
{
    template<typename Base, typename Final, typename... Dependencies>
    class System :
        public keng::core::RefCountImpl<Base>,
        public DependenciesContainer<Dependencies...>
    {
    public:
        virtual const char* GetSystemName() const override {
            return Final::SystemName();
        }

        virtual bool ForEachDependency(const edt::Delegate<bool(const char*)>& delegate) const override {
            return CallAndRethrowM + [&]() -> bool {
                UnusedVar(delegate);
                return (delegate.Invoke(Dependencies::SystemName()) || ... || false);
            };
        }
    };
}
