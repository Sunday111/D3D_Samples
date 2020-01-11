#pragma once

#include "EverydayTools/Preprocessor/ExpotImport.h"

#ifdef KENG_FILESYSTEM_DLL
#define KENG_FILESYSTEM_API EXPORT
#else
#define KENG_FILESYSTEM_API IMPORT
#endif
