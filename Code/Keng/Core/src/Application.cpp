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
    }

    void Application::LoadModule(std::string_view name) {
        CallAndRethrowM + [&] {

            auto module = ModulePtr::MakeInstance(name);
            if (module->IsGlobalModule()) {
                GlobalEnvironment::PrivateInstance().RegisterModule(module);
            } else {
                m_modules.push_back(ModulePtr::MakeInstance(name));
            }
        };
    }

    void Application::LoadDependencies() {
        bool addedNewSystem;

        do {
            addedNewSystem = false;
            for (auto& module : m_modules) {
                auto& system = module->GetSystem();

                auto onDependency = [&](const char* dependencyName) {
                    if (FindSystem(dependencyName)) {
                        return false;
                    }

                    LoadModule(dependencyName);
                    return true;
                };

                edt::Delegate<bool(const char*)> delegate;
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
        for (auto& moduleToLoad : params.modulesToLoad) {
            LoadModule(moduleToLoad);
        }

        LoadDependencies();

        std::vector<ISystemPtr> walkQueue;
        std::vector<ISystemPtr> systems;

        for (auto& module : m_modules) {
            systems.push_back(module->GetSystem());
        }

        auto findSystem = [&systems](std::string_view name) -> ISystemPtr {
            for (auto& system : systems) {
                if (system->GetSystemName() == name) {
                    return system;
                }
            }

            auto& gEnv = GlobalEnvironment::PrivateInstance();
            auto system = gEnv.TryGetSystem(name);
            if (system) {
                return system;
            }

            return nullptr;
        };

        std::stable_sort(m_modules.begin(), m_modules.end(), [&](const ModulePtr& ma, const ModulePtr& mb) {
            walkQueue.clear();

            auto a = ma->GetSystem();
            auto b = mb->GetSystem();

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

                    auto system = findSystem(name);
                    edt::ThrowIfFailed(system != nullptr, "Found dependency that was not registered as system");
                    walkQueue.push_back(system);
                    return false;
                };

                edt::Delegate<bool(const char*)> delegate;
                delegate.Bind(onDependency);

                dependencyFound = current->ForEachDependency(delegate);
            }

            return dependencyFound;
        });

        IApplicationPtr thisPtr(this);
        for (const ModulePtr& module : m_modules) {
            module->GetSystem()->Initialize(thisPtr);
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
                auto sleep = m_fpsCounter.GetDesiredFrameDuration() - average;
                if (sleep.count() > 0) {
                    //std::this_thread::sleep_for(sleep); <-- so slow!
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

    ISystemPtr Application::FindSystem(const char* name) const {
        std::string_view view(name);
        for (auto& module : m_modules) {
            auto& system = module->GetSystem();
            if (system->GetSystemName() == view) {
                return system;
            }
        }

        auto& gEnv = GlobalEnvironment::PrivateInstance();
        auto system = gEnv.TryGetSystem(name);
        if (system) {
            return system;
        }

        return nullptr;
    }

    void Application::Shutdown() {
        for (auto it = m_modules.rbegin(); it != m_modules.rend(); ++it) {
            (*it)->GetSystem()->Shutdown();
        }

        while (!m_modules.empty()) {
            //assert(m_modules.back()->GetSystem()->GetReferencesCount() == 1);
            m_modules.pop_back();
        }

        GlobalEnvironment::PrivateInstance().DestroyApplication(this);
    }

}
