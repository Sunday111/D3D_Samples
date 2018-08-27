#pragma once

#include "Keng/Core/FwdDecl.h"
#include "EverydayTools/Delegate.h"

namespace keng::core
{
    class ISystem : public IRefCountObject
    {
    public:
        virtual void Initialize(const IApplicationPtr& app) = 0;
        virtual bool Update() = 0;
        virtual void Shutdown() = 0;
        virtual const char* GetSystemName() const = 0;
        virtual bool ForEachDependency(const edt::Delegate<bool(const char*)>& delegate) const = 0;
        virtual ~ISystem() = default;
    };
}
