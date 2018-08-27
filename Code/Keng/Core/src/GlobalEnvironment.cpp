#include "Application.h"
#include "GlobalEnvironment.h"
#include "EverydayTools/Exception/CallAndRethrow.h"
#include "EverydayTools/Exception/ThrowIfFailed.h"
#include "Keng/Core/IGlobalSystem.h"
#include <memory>
#include <algorithm>

namespace keng::core
{
    GlobalEnvironment::GlobalEnvironment() = default;
    GlobalEnvironment::~GlobalEnvironment() = default;

    IGlobalSystem& GlobalEnvironment::GetGlobalSystem(size_t id) {
        return CallAndRethrowM + [&] () -> IGlobalSystem& {
            edt::ThrowIfFailed(id < m_modules.size());
            return *std::static_pointer_cast<IGlobalSystem>(m_modules[id]->GetSystem());
        };
    }

    void GlobalEnvironment::RegisterModule(ModulePtr module) {
        std::lock_guard<std::mutex>l(m_mutex);
        CallAndRethrowM + [&] {
            auto system = std::dynamic_pointer_cast<IGlobalSystem>(module->GetSystem());
            m_modules.push_back(module);
        };
    }

    GlobalEnvironment& GlobalEnvironment::PrivateInstance() {
        static GlobalEnvironment instance;
        return instance;
    }

    IGlobalEnvironment& IGlobalEnvironment::Instance() {
        return GlobalEnvironment::PrivateInstance();
    }

    void GlobalEnvironment::DestroyApplication(Application* app) {
        std::lock_guard<std::mutex>l(m_mutex);
        auto threadId = std::this_thread::get_id();

        {
            auto it = m_threadToApplication.find(threadId);
            assert(it != m_threadToApplication.end());
            if (it != m_threadToApplication.end()) {
                m_threadToApplication.erase(it);
            }
        }

        assert(!m_applications.empty());
        if(!m_applications.empty()) {
            auto it = std::find_if(m_applications.begin(), m_applications.end(), [app](const ApplicationPtr& p) {
                return p.Get() == app;
            });
            assert(it != m_applications.end());
            if (it != m_applications.end()) {
                std::swap(*it, m_applications.back());
                m_applications.resize(m_applications.size() - 1);
            }
        }
    }

    ISystemPtr GlobalEnvironment::TryGetSystem(std::string_view name) {
        std::lock_guard<std::mutex>l(m_mutex);
        for (auto& module : m_modules) {
            if (module->GetSystem()->GetSystemName() == name) {
                return module->GetSystem();
            }
        }
        return nullptr;
    }

    Application* GlobalEnvironment::CreateApplication() {
        std::lock_guard<std::mutex>l(m_mutex);
        auto threadId = std::this_thread::get_id();
        edt::ThrowIfFailed(m_threadToApplication.find(threadId) == m_threadToApplication.end());
        m_applications.push_back(ApplicationPtr::MakeInstance());
        auto it = m_threadToApplication.insert(std::make_pair(threadId, m_applications.back().Get()));
        return it.first->second.Get();
    }
}