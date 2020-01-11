#include "TexturedQuadSample.h"
#include "EverydayTools/Preprocessor/ExpotImport.h"

extern "C"
{
    void EXPORT __cdecl CreateSystem(void** result) {
        *result = new textured_quad_sample::TexturedQuadSample();
    }
}
