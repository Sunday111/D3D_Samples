#pragma once

#include <tuple>

#include "EverydayTools/Exception/CallAndRethrow.h"
#include "ISystem.h"

namespace keng::core
{
    namespace impl
    {
        template<typename Head = void, typename... Tail>
        class GetPointersTuple
        {
        public:
            using HeadTuple = std::tuple<Ptr<Head>>;
            using TailTuple = typename GetPointersTuple<Tail...>::Type;
            using Type = decltype(std::tuple_cat(std::declval<HeadTuple>(), std::declval<TailTuple>()));
        };

        template<typename Head>
        class GetPointersTuple<Head>
        {
        public:
            using Type = std::tuple<Ptr<Head>>;
        };

        template<>
        class GetPointersTuple<void>
        {
        public:
            using Type = std::tuple<>;
        };
    }
        
    template<typename... Dependencies>
    class DependenciesContainer
    {
    public:
        void StoreDependencies(IApplication& app) {
            CallAndRethrowM + [&] {
                (StoreDependency<Dependencies>(app), ...);
            };
        }
    
        template<typename T>
        T& GetSystem() {
            return *std::get<Ptr<T>>(m_storage);
        }
    
    private:
        template<typename T>
        void StoreDependency(IApplication& app) {
            CallAndRethrowM + [&] {
                auto found = app.FindSystem<T>();
                edt::ThrowIfFailed(found != nullptr, "Dependency ", T::SystemName(), " is not found");
                std::get<Ptr<T>>(m_storage) = found;
            };
        }

        using DependenciesPointers = typename impl::GetPointersTuple<Dependencies...>::Type;
        DependenciesPointers m_storage;
    };
}
