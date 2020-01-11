#include "FourierPrinterSystem.h"
#include "EverydayTools/Preprocessor/ExpotImport.h"

extern "C"
{
    void EXPORT __cdecl CreateSystem(void** result) {
        *result = new simple_quad_sample::FourierPrinterSystem();
    }
}
