#include "WinWrappers/WinWrappers.h"
#include "EverydayTools/Array/ArrayView.h"
#include "EverydayTools/Array/ArrayViewVector.h"
#include "EverydayTools/UnusedVar.h"
#include "EverydayTools/Exception/CallAndRethrow.h"
#include "EverydayTools/Exception/ThrowIfFailed.h"
#include "EverydayTools/Exception/CheckedCast.h"
#include <algorithm>
#include <string_view>
#include <string>
#include <sstream>
#include <vector>
#include <thread>
#include <future>

#include "Keng/Core/IApplication.h"
#include "Keng/Core/CreateApplication.h"

namespace {
    template<typename T>
    class LocalFreeDeleter
    {
    public:
        void operator()(T* ptr) {
            if (ptr) {
                LocalFree(ptr);
            }
        }
    };
    
    std::string ConvertString(const std::wstring& wide) {
        std::string result;
        result.reserve(wide.size());
        std::transform(wide.begin(), wide.end(), std::back_inserter(result), edt::CheckedCast<char, wchar_t>);
        return result;
    }

    template<typename T>
    using WinLocalPtr = std::unique_ptr<T, LocalFreeDeleter<T>>;

    int main() {

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

                std::string modulesToLoad[1];
                modulesToLoad[0] = ConvertString(libraryName);

                std::vector<const char*> rawNames;
                for (auto& str : modulesToLoad) {
                    rawNames.push_back(str.data());
                }

                keng::core::ApplicationStartupParameters params;
                params.modulesToLoad = edt::MakeArrayView(rawNames);

                app->Initialize(params);
                app->Run();
                app->Shutdown();

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
}

INT WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, INT nCmdShow) {
    UnusedVar(hInstance, hPrevInstance, lpCmdLine, nCmdShow);

    std::future<int> a = std::async(std::launch::async, main);
    std::future<int> b = std::async(std::launch::async, main);

    auto result_a = a.get();
    auto result_b = b.get();

    return result_a + result_b;
}
