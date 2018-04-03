#pragma once

#ifdef KENG_MEMORY_DLL
    #ifdef _MSC_VER
        #define KENG_MEMORY_API __declspec(dllexport)
    #else
        static_assert(false, "Unknown compiler");
    #endif
#else
    #ifdef _MSC_VER
        #define KENG_MEMORY_API __declspec(dllimport)
    #else
        static_assert(false, "Unknown compiler");
    #endif
#endif