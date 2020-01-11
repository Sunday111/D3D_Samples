#pragma once

#include "EverydayTools/Preprocessor/ExpotImport.h"

#ifdef KENG_CORE_DLL
#define KENG_CORE_API EXPORT
#else
#define KENG_CORE_API IMPORT
#endif
