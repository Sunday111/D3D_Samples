#pragma once

#include "Keng/Core/FwdDecl.h"
#include "WinWrappers/WinWrappers.h"
#include <string_view>

namespace keng::core
{
    class Module : public RefCountImpl<IRefCountObject>
    {
    public:
        using CreateSystemSignature = void(__cdecl*)(void**);

        Module(std::string_view name);
        ~Module();

        const ISystemPtr& GetSystem() const;
        bool IsGlobalModule() const;

    private:
        static HMODULE Load(std::string_view name);
        void Check() const;
        void CreateSystem();
        void* GetFunctionAddress(std::string_view name) const;

    private:
        ISystemPtr m_system;
        HMODULE m_handle = nullptr;
    };
}
