#pragma once

#include "Keng/Core/FwdDecl.h"
#include "EverydayTools/Exception/CallAndRethrow.h"
#include <string_view>

namespace keng::core
{
    class ISystem : public IRefCountObject
    {
    public:
        virtual void Initialize(const IApplicationPtr& app) = 0;
        virtual bool Update() = 0;
        virtual std::string_view GetSystemName() const = 0;
        virtual bool ForEachSystemDependency(bool(*pfn)(std::string_view systemName, void* context), void* context) const = 0;
        virtual ~ISystem() = default;

        template<typename Functor>
        bool ForEachSystemDependency(Functor&& fn) {
            return CallAndRethrowM + [&] {
                return ForEachSystemDependency([](std::string_view systemGUID, void* context) -> bool {
                    return (*((Functor*)context))(systemGUID);
                }, &fn);
            };
        }
    };
}
