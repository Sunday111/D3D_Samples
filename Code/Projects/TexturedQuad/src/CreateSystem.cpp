#include "TexturedQuadSample.h"

extern "C"
{
    void __declspec(dllexport) __cdecl CreateSystem(void** result) {
        *result = new textured_quad_sample::TexturedQuadSample();
    }
}
