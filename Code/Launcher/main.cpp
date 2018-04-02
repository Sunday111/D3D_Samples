#include "WinWrappers/WinWrappers.h"
#include "EverydayTools/Array/ArrayView.h"
#include "EverydayTools/UnusedVar.h"
#include "EverydayTools/Exception/CallAndRethrow.h"
#include "EverydayTools/Exception/ThrowIfFailed.h"
#include <string_view>
#include <string>
#include <sstream>

#include "Keng/Core/Application.h"

using SystemCreatorSignature = void(__cdecl*)(void** );

int Run(SystemCreatorSignature createFn) {
    using namespace keng;
    using namespace core;

    void* rawSystem = nullptr;
    createFn(&rawSystem);

    auto system = reinterpret_cast<ISystem*>(rawSystem);

    Application app;
    app.AddSystem(system);
    app.Initialize();
    app.Run();

    return 0;
}

template<typename T>
struct LocalFreeDeleter
{
    void operator()(T* ptr) {
        if (ptr) {
            LocalFree(ptr);
        }
    }
};

template<typename T>
using WinLocalPtr = std::unique_ptr<T, LocalFreeDeleter<T>>;

INT WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, INT nCmdShow) {
    UnusedVar(hInstance, hPrevInstance, lpCmdLine, nCmdShow);

    try {
        using WA = WinAPI<wchar_t>;
        return CallAndRethrowM + [&] {
            auto cmdLine = GetCommandLineW();
            int nArguments;
            WinLocalPtr<LPWSTR> argumentsPtr(CommandLineToArgvW(cmdLine, &nArguments));
            edt::DenseArrayView<LPWSTR> arguments(argumentsPtr.get(), nArguments);
            std::wstring libraryName;

            if (nArguments > 1) {
                libraryName = arguments[1];
            } else {
                const char* filters[]{
                    "DLL", "*.dll",
                    nullptr, nullptr
                };

                wchar_t szFile[MAX_PATH];

                // open a file name
                OpenFileName<wchar_t> ofn;
                ZeroMemory(&ofn, sizeof(ofn));
                ofn.lStructSize = sizeof(ofn);
                ofn.hwndOwner = NULL;
                ofn.lpstrFile = szFile;
                ofn.lpstrFile[0] = '\0';
                ofn.nMaxFile = sizeof(szFile);
                ofn.lpstrFilter = L"DLL\0*.dll\0";
                ofn.nFilterIndex = 1;
                ofn.lpstrFileTitle = NULL;
                ofn.nMaxFileTitle = 0;
                ofn.lpstrInitialDir = NULL;
                ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
                edt::ThrowIfFailed(WA::GetOpenFileName_(&ofn), "You have to select library to load!");
                libraryName = ofn.lpstrFile;
            }

            auto module = WA::LoadLibrary_(libraryName.c_str());
            edt::ThrowIfFailed(module != nullptr, L"Failed to load library!");

            std::string_view runFunctionName = "CreateSystem";
            auto runFn = (SystemCreatorSignature)GetProcAddress(module, runFunctionName.data());
            edt::ThrowIfFailed(runFn, "Failed to get \"", runFunctionName, "\" function!");
            return Run(runFn);
        };
    } catch (const std::exception& ex) {
        using WA = WinAPI<char>;
        std::stringstream sstream;
        PrintException(sstream, ex);
        WA::MessageBox_(nullptr, sstream.str().c_str(), "Error", MB_ICONERROR);
    } catch (...) {
        using WA = WinAPI<char>;
        WA::MessageBox_(nullptr, "Unexpected error!", "Error", MB_ICONERROR);
    }
    return 1;
}
