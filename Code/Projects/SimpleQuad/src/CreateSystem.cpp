#include "SampleSystem.h"

extern "C"
{
    void __declspec(dllexport) __cdecl CreateSystem(void** result) {
        *result = new simple_quad_sample::SampleSystem();
    }
}