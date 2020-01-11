#include "RenderTextSample.h"
#include "EverydayTools/Preprocessor/ExpotImport.h"

extern "C"
{
    void EXPORT __cdecl CreateSystem(void** result) {
        *result = new render_text_sample::RenderTextSample();
    }
}
