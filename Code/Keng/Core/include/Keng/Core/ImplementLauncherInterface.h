#pragma once

#include "WinWrappers/WinWrappers.h"
#include "EverydayTools/Exception/CallAndRethrow.h"
#include "EverydayTools/Exception/PrintException.h"

namespace keng::core
{
    using LauncherSignature = int(__cdecl*)(HINSTANCE hInstance, HINSTANCE hPrevInstance, char* cmdLine, int nCmdShow);

    template<LauncherSignature pfn>
    int WrapLauncerCall(HINSTANCE hInstance, HINSTANCE hPrevInstance, char* cmdLine, int nCmdShow) {
        using TChar = char;
        using WA = WinAPI<TChar>;
        try {
            return pfn(hInstance, hPrevInstance, cmdLine, nCmdShow);
        } catch (const std::exception& ex) {
            std::stringstream sstream;
            PrintException(sstream, ex);
            WA::MessageBox_(nullptr, sstream.str().c_str(), "Error", MB_ICONERROR);
        } catch (...) {
            WA::MessageBox_(nullptr, "Unexpected error!", "Error", MB_ICONERROR);
        }

        return 1;
    }

}

#define MAKE_LAUNCHER_ENTRY_POINT($function)                                                                                                \
    extern "C" {                                                                                                                            \
        int __declspec(dllexport) __cdecl RunApplication(HINSTANCE hInstance, HINSTANCE hPrevInstance, char* cmdLine, int nCmdShow) {       \
            return keng::core::WrapLauncerCall<$function>(hInstance, hPrevInstance, cmdLine, nCmdShow);                                     \
        }                                                                                                                                   \
    }
