#include "ProjectECS.h"
#include "EverydayTools/Preprocessor/ExpotImport.h"

extern "C"
{
    void EXPORT __cdecl CreateSystem(void** result) {
        *result = new project_ecs::ProjectECS();
    }
}
