#pragma once

#include "WinWrappers/WinWrappers.h"
#include "EverydayTools/Exception/CallAndRethrow.h"
#include "EverydayTools/Exception/ThrowIfFailed.h"
#include "Keng/Core/IApplication.h"

namespace keng::core
{
    using CreateApplicationFn = void(__cdecl*)(void**);

    inline CreateApplicationFn GetCreateAppFunction(HMODULE module) {
        return CallAndRethrowM + [&] {
            const char* functionName = "CreateApplication";
            auto result = (CreateApplicationFn)GetProcAddress(module, functionName);
            edt::ThrowIfFailed(result != nullptr, "Failed to get \"", functionName, "\" function");
            return result;
        };
    }

    inline IApplicationPtr CreateApplication(HMODULE module) {
        return CallAndRethrowM + [&] {
            void* rawPointer = nullptr;
            auto fn = GetCreateAppFunction(module);
            fn(&rawPointer);
            edt::ThrowIfFailed(rawPointer != nullptr, "Failed to create application");
            return IApplicationPtr(reinterpret_cast<IApplication*>(rawPointer));
        };
    }
}