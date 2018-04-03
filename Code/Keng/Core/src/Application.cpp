#include "Application.h"
#include "EverydayTools/Exception/CallAndRethrow.h"
#include "EverydayTools/Exception/ThrowIfFailed.h"
#include "WinWrappers/WinWrappers.h"
#include <algorithm>
#include "WinWrappers/WinWrappers.h"

namespace keng::core
{
    using SystemCreatorSignature = void(__cdecl*)(void**);

    void Application::SetVSync(bool value) {
        m_vSync = value;
    }

    void Application::LoadModule(std::string_view name) {
        using WA = WinAPI<char>;
        auto module = WA::LoadLibrary_(name.data());
        edt::ThrowIfFailed(module != nullptr, L"Failed to load library!");

        std::string_view runFunctionName = "CreateSystem";
        auto createFn = (SystemCreatorSignature)GetProcAddress(module, runFunctionName.data());
        edt::ThrowIfFailed(createFn, "Failed to get \"", runFunctionName, "\" function!");

        void* rawSystem = nullptr;
        createFn(&rawSystem);

        std::unique_ptr<ISystem> system;
        system.reset(reinterpret_cast<ISystem*>(rawSystem));
        m_systems.push_back(std::move(system));
    }

    void Application::LoadDependencies() {
        bool addedNewSystem;

        do
        {
            addedNewSystem = false;
            for (auto& system : m_systems) {
                addedNewSystem = system->ForEachSystemDependency([&](std::string_view dependencyName) {
                    if (FindSystem(dependencyName)) {
                        return false;
                    }

                    LoadModule(dependencyName);
                    return true;
                });

                if (addedNewSystem) {
                    break;
                }
            }
        } while (addedNewSystem);
    }

    void Application::Initialize(const ApplicationStartupParameters& params) {
        for (auto& moduleToLoad: params.modulesToLoad) {
            LoadModule(moduleToLoad);
        }

        LoadDependencies();

        std::vector<ISystem*> walkQueue;
        std::vector<ISystem*> initQueue;

        for (auto& sys : m_systems) {
            initQueue.push_back(sys.get());
        }

        std::stable_sort(initQueue.begin(), initQueue.end(), [&](ISystem* a, ISystem* b) {
            walkQueue.clear();

            auto a_name = a->GetSystemName();
            bool dependencyFound = false;
            walkQueue.push_back(b);
            while (!dependencyFound && !walkQueue.empty()) {
                auto current = walkQueue.back();
                walkQueue.pop_back();
                dependencyFound = current->ForEachSystemDependency([&](std::string_view name) {
                    if (a_name == name) {
                        return true;
                    }

                    auto sysIt = std::find_if(m_systems.begin(), m_systems.end(), [name](const std::unique_ptr<ISystem>& sys) {
                        return std::string_view(sys->GetSystemName()) == name;
                    });
                    edt::ThrowIfFailed(sysIt != m_systems.end(), "Found dependency that was not registered as system");
                    walkQueue.push_back(sysIt->get());
                    return false;
                });
            }

            return dependencyFound;
        });

        for (auto& system : initQueue) {
            system->Initialize(this);
        }
    }

    bool Application::Update() {
        return CallAndRethrowM + [&] {
            if (m_vSync) {
                using namespace std::chrono;
                auto t0 = high_resolution_clock::now();
                auto retVal = UpdateSystems();
                auto t1 = high_resolution_clock::now();
                auto average = m_fpsCounter.CalcAverageTick(t1 - t0);
                auto sleep = TFrameRateCounter::GetDesiredFrameDuration(DesiredFPS) - average;
                if (sleep.count() > 0) {
                    //std::this_thread::sleep_for(sleep);
                    Sleep(sleep);
                }
                return retVal;
            } else {
                return UpdateSystems();
            }
        };
    }

    void Application::Run() {
        CallAndRethrowM + [&] {
            while (Update());
        };
    }

    keng::core::ISystem* Application::FindSystem(std::string_view name) const
    {
        for (auto& sys: m_systems) {
            if (sys->GetSystemName() == name) {
                return sys.get();
            }
        }

        return nullptr;
    }

}
