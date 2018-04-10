#pragma once

#include "Keng/Core/FwdDecl.h"
#include "EverydayTools/Exception/CallAndRethrow.h"
#include <string_view>
#include "EverydayTools/Delegate.h"

namespace keng::core
{
    class ISystem : public IRefCountObject
    {
    public:
        virtual void Initialize(const IApplicationPtr& app) = 0;
        virtual bool Update() = 0;
        virtual std::string_view GetSystemName() const = 0;
        virtual bool ForEachDependency(const edt::Delegate<bool(std::string_view)>& delegate) const = 0;
        virtual ~ISystem() = default;
    };
}
