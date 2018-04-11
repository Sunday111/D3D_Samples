#pragma once

#ifdef _MSC_VER
    #ifdef KENG_CORE_DLL
        #define KENG_CORE_API _declspec(dllexport)
    #else
        #define KENG_CORE_API _declspec(dllimport)
    #endif
#else
static_assert(false, "Not supported compiler");
#endif
