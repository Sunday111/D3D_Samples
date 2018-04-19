#include "Application.h"
#include "EverydayTools/Exception/CallAndRethrow.h"
#include "EverydayTools/Exception/ThrowIfFailed.h"
#include "Keng/Core/ISystem.h"
#include "WinWrappers/WinWrappers.h"
#include <algorithm>
#include "WinWrappers/WinWrappers.h"
#include "Keng/Core/ISystem.h"
#include "GlobalEnvironment.h"

namespace keng::core
{
    using SystemCreatorSignature = void(__cdecl*)(void**);

    void Application::SetVSync(bool value) {
        m_vSync = value;
    }

    Application::Application() {
        GlobalEnvironment::Initialize();
    }

    void Application::LoadModule(std::string_view name) {
        CallAndRethrowM + [&] {
            m_modules.push_back(ModulePtr::MakeInstance(name));
        };
    }

    void Application::LoadDependencies() {
        bool addedNewSystem;

        do
        {
            addedNewSystem = false;
            for (auto& module : m_modules) {
                auto& system = module->GetSystem();

                auto onDependency = [&](std::string_view dependencyName) {
                    if (FindSystem(dependencyName)) {
                        return false;
                    }

                    LoadModule(dependencyName);
                    return true;
                };

                edt::Delegate<bool(std::string_view)> delegate;
                delegate.Bind(onDependency);

                addedNewSystem = system->ForEachDependency(delegate);

                if (addedNewSystem) {
                    break;
                }
            }
        } while (addedNewSystem);
    }

    bool Application::UpdateSystems() {
        return CallAndRethrowM + [&] {
            bool finished = false;
            for (auto& module : m_modules) {
                if (auto& system = module->GetSystem()) {
                    if (!system->Update()) {
                        finished = true;
                    }
                }
            }
            return !finished;
        };
    }

    void Application::Initialize(const ApplicationStartupParameters& params) {
        for (auto& moduleToLoad: params.modulesToLoad) {
            LoadModule(moduleToLoad);
        }

        LoadDependencies();

        std::vector<ISystemPtr> walkQueue;
        std::vector<ISystemPtr> initQueue;

        for (auto& module : m_modules) {
            initQueue.push_back(module->GetSystem());
        }

        std::stable_sort(initQueue.begin(), initQueue.end(), [&](const ISystemPtr& a, const ISystemPtr& b) {
            walkQueue.clear();

            auto a_name = a->GetSystemName();
            bool dependencyFound = false;
            walkQueue.push_back(b);
            while (!dependencyFound && !walkQueue.empty()) {
                auto current = walkQueue.back();
                walkQueue.pop_back();

                auto onDependency = [&](std::string_view name) {
                    if (a_name == name) {
                        return true;
                    }

                    auto system = FindSystem(name);
                    edt::ThrowIfFailed(system != nullptr, "Found dependency that was not registered as system");
                    walkQueue.push_back(system);
                    return false;
                };

                edt::Delegate<bool(std::string_view)> delegate;
                delegate.Bind(onDependency);

                dependencyFound = current->ForEachDependency(delegate);
            }

            return dependencyFound;
        });

        IApplicationPtr thisPtr(this);
        for (auto& system : initQueue) {
            system->Initialize(thisPtr);
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

    ISystemPtr Application::FindSystem(std::string_view name) const
    {
        for (auto& module: m_modules) {
            auto& system = module->GetSystem();
            if (system->GetSystemName() == name) {
                return system;
            }
        }

        return nullptr;
    }

    void Application::Shutdown() {
        GlobalEnvironment::Destroy();
        m_modules.clear();
    }

}
