#include "GlobalEnvironment.h"

#include "EverydayTools/Preprocessor/ExpotImport.h"

extern "C"
{
    void EXPORT __cdecl CreateApplication(void** result) {
        using namespace keng::core;
        *result = GlobalEnvironment::PrivateInstance().CreateApplication();
    }
}
