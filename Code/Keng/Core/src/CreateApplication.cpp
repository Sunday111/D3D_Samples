#include "GlobalEnvironment.h"

extern "C"
{
    void __declspec(dllexport) __cdecl CreateApplication(void** result) {
        using namespace keng::core;
        *result = GlobalEnvironment::PrivateInstance().CreateApplication();
    }
}
