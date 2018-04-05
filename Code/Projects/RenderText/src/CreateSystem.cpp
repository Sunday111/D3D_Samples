#include "System.h"

extern "C"
{
    void __declspec(dllexport) __cdecl CreateSystem(void** result) {
        *result = new render_text_sample::System();
    }
}
