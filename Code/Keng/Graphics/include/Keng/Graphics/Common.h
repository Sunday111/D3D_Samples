#pragma once

#ifdef _MSC_VER 
    #ifdef KENG_GRAPHICS_SYSTEM_DLL
        #define KENG_GRAPHICS_SYSTEM_API __declspec(dllexport)
    #else
        #define KENG_GRAPHICS_SYSTEM_API __declspec(dllimport)
    #endif
#else
    static_assert(false, "This compiler is not supported")
#endif
