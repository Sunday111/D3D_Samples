#pragma once

#include "Keng/Core/FwdDecl.h"
#include "EverydayTools/Delegate.h"

namespace keng::core
{
    class ISystem : public IRefCountObject
    {
    public:
        virtual void OnSystemEvent(const IApplicationPtr&, const SystemEvent&) {}
        virtual const char* GetSystemName() const = 0;
        virtual bool ForEachDependency(const edt::Delegate<bool(const char*)>& delegate) const = 0;
        virtual ~ISystem() = default;
    };
}
