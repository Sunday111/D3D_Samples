#include "WindowSystem.h"

extern "C"
{
    void __declspec(dllexport) __cdecl CreateSystem(void** result) {
        *result = new keng::window_system::WindowSystem();
    }
}
