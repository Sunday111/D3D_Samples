#pragma once

#include "EverydayTools/Preprocessor/ExpotImport.h"

#ifdef KENG_MEMORY_DLL
    #define KENG_MEMORY_API EXPORT
#else
    #define KENG_MEMORY_API IMPORT
#endif
