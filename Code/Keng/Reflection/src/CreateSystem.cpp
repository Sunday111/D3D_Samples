#include "ReflectionSystem.h"

extern "C"
{
    void __declspec(dllexport) __cdecl CreateSystem(void** result) {
        *result = new keng::reflection::ReflectionSystem();
    }
}
