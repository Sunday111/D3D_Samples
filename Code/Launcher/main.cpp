#include "WinWrappers/WinWrappers.h"
#include "EverydayTools/Array/ArrayView.h"
#include "EverydayTools/UnusedVar.h"
#include "EverydayTools/Exception/CallAndRethrow.h"
#include "EverydayTools/Exception/ThrowIfFailed.h"
#include <string_view>
#include <string>
#include <sstream>

#include "Keng/Core/IApplication.h"
#include "Keng/Core/CreateApplication.h"

template<typename T>
struct LocalFreeDeleter
{
    void operator()(T* ptr) {
        if (ptr) {
            LocalFree(ptr);
        }
    }
};

static std::string ConvertString(const std::wstring& wide) {
    std::string result;
    result.reserve(wide.size());
    for (wchar_t letter : wide) {
        edt::ThrowIfFailed(
            letter <= std::numeric_limits<char>::max() &&
            letter >= std::numeric_limits<char>::min(),
            "Only ASCII names supported"
        );

        result.push_back(static_cast<char>(letter));
    }

    return result;
}

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

            auto module = WA::LoadLibrary_(L"KengCore");
            edt::ThrowIfFailed(module != nullptr, L"Failed to load library!");

            auto app = keng::core::CreateApplication(module);

            keng::core::ApplicationStartupParameters params;
            params.modulesToLoad.push_back(ConvertString(libraryName));

            app->Initialize(params);
            app->Run();

            return 0;
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
