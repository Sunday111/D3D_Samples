#include "Application.h"

extern "C"
{
    void __declspec(dllexport) __cdecl CreateApplication(void** result) {
        *result = new keng::core::Application();
    }
}
