#pragma once

#ifdef _MSC_VER
    #ifdef KENG_FILESYSTEM_DLL
        #define KENG_FILESYSTEM_API _declspec(dllexport)
    #else
        #define KENG_FILESYSTEM_API _declspec(dllimport)
    #endif
#else
static_assert(false, "Not supported compiler");
#endif
