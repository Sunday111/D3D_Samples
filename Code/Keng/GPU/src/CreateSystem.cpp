#include "GPUSystem.h"
#include "EverydayTools/Preprocessor/ExpotImport.h"

extern "C"
{
    void EXPORT __cdecl CreateSystem(void** result) {
        *result = new keng::graphics::gpu::GPUSystem();
    }
}
